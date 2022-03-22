import logging

from PIL import Image
from django.conf import settings
from django.contrib.auth.signals import user_logged_in
from django.core.files.storage import default_storage
from django.urls import reverse
from django.utils.timezone import now
from django.utils.translation import ugettext as _
from django.http import QueryDict
from rest_framework.exceptions import ValidationError
from rest_framework.parsers import MultiPartParser, FormParser, JSONParser
from rest_framework.renderers import JSONRenderer, TemplateHTMLRenderer
from rest_framework.response import Response
from rest_framework.views import APIView
from ik.api.permissions import IsAuthenticated
from ik.api.serializers.chat import SendMessageSerializer, CreatePrivateChatSerializer, CreateGroupChatSerializer, ForwardMessageSerializer
from ik.messengers.client import get_chat_client
from ik.messengers.chatcube.client import IKChatcubeClient
from ik.messengers.utils import split_messeger_id
from ik.messengers.telegram.client import TelegramException
from ik.models import Member
from ik.models.chat import OpenedChat
from ik.utils.site import build_absolute_uri
from ik.constants import *
from ik import constants

logger = logging.getLogger("cc")

DEFAULT_LOAD_MESSAGES_LIMIT = 20

class SendMessageView(APIView):
    PHOTO_MIN_WIDTH = 10
    PHOTO_MIN_HEIGHT = 10

    permission_classes = (IsAuthenticated,)
    renderer_classes = (JSONRenderer,)
    parser_classes = (JSONParser, MultiPartParser, FormParser)

    def post(self, request, *args, **kwargs):
        serializer = SendMessageSerializer(data=request.data)
        serializer.is_valid(raise_exception=True)
        validated_data = serializer.validated_data

        chat_id = validated_data.get('chat_id')
        recipient_id = validated_data.get('recipient_id')
        type = validated_data['type']
        text = validated_data.get('text', '').rstrip(" \t")
        file = validated_data.get('file')
        file_name = validated_data.get('file_name')
        file_type = validated_data.get('file_type', 0)

        if not chat_id and not recipient_id:
            raise ValidationError("You must set the 'chat_id' or 'recipient_id' fields")

        if type == MESSAGE_TYPE_TEXT:
            if text == '':
                raise ValidationError(_("Field \"{0}\" can't be empty").format("text"))

        if type == MESSAGE_TYPE_FILE:
            if not file:
                raise ValidationError(_("Field \"{0}\" can't be empty").format("file"))

        if type == MESSAGE_TYPE_PHOTO:
            if not file:
                raise ValidationError(_("Field \"{0}\" can't be empty").format("file"))
            try:
                img = Image.open(file)
                if img.width < self.PHOTO_MIN_WIDTH or img.height < self.PHOTO_MIN_HEIGHT:
                    logger.error("Uploaded image is too small, file: {}, {}x{}".format(file, img.width, img.height))
                    raise ValidationError(_("Uploaded image is too small, must be at least {}x{}").format(self.PHOTO_MIN_WIDTH, self.PHOTO_MIN_HEIGHT))
            except Exception as ex:
                logger.error("Uploaded file is not an image or image broken, file: {}, {}".format(file, ex))
                raise ValidationError(
                    _("Uploaded file is not an image or image broken"))

        if type == MESSAGE_TYPE_STICKER:
            if text == '':
                raise ValidationError(_("Field \"{0}\" can't be empty").format("text"))

            if not default_storage.exists(text):
                raise ValueError("Missing sticker image: {}".format(text))


        me = request.user
        if chat_id:
            messenger = chat_id[0]
        else:
            messenger = recipient_id[0]

        client = get_chat_client(messenger, me)

        if chat_id:
            chat_id = chat_id[1:]
            chat = client.get_chat(chat_id, full_info=False)
        else:
            recipient_id = recipient_id[1:]
            chat = client.get_or_create_private_chat(recipient_id, full_info=False)

        client.check_send_permissions(chat)

        if type == MESSAGE_TYPE_TEXT:
            client.send_message_text(chat, text, reply_to_id=validated_data.get('reply_to_id'))
        elif type == MESSAGE_TYPE_PHOTO:
            client.send_message_photo(chat, file, caption=text, reply_to_id=validated_data.get('reply_to_id'))
        elif type == MESSAGE_TYPE_FILE:
            client.send_message_file(chat, file, caption=text, file_name=file_name, file_type=file_type, reply_to_id=validated_data.get('reply_to_id'))
        elif type == MESSAGE_TYPE_STICKER:
            client.send_message_sticker(chat, text, reply_to_id=validated_data.get('reply_to_id'))
        else:
            raise ValidationError("Unknown message type")

        return Response({"result": "ok"})


class ChatsListView(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        reqdata = self.request.GET
        initial = reqdata.get('initial')
        me = request.user
        if initial:
            logger.info(u"Member {} loads initial chat".format(me))

            app_ver = reqdata.get('app_version')
            if app_ver:
                # tz = reqdata.get('timezone')
                # if tz:
                #     request.auth.timezone = get_app_timezone(tz, reqdata.get('utc_offset'))
                request.auth.app_version = app_ver
                request.auth.was_online = now()
                request.auth.save()

            me.last_login = now()
            me.save(update_fields=['last_login'])
            user_logged_in.send(Member, user=me, request=request)

        cc_client = get_chat_client(MESSENGER_CHATCUBE, me)
        chats = cc_client.get_chats_as_dict()

        tg_client = get_chat_client(MESSENGER_TG, me, raise_exception=False)
        if tg_client:
            try:
                chats.extend(tg_client.get_chats_as_dict())
            except TelegramException as ex:
                logger.warning("Telegram error, TG chats not available: {}.".format(ex))

        return Response(chats)


class ChatCreatePrivateView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        serializer = CreatePrivateChatSerializer(data=request.data)
        serializer.is_valid(raise_exception=True)
        validated_data = serializer.validated_data

        messenger, mid = split_messeger_id(validated_data['member_id'])

        client = get_chat_client(messenger, request.user)
        chat = client.get_or_create_private_chat(mid, full_info=True)
        if not isinstance(chat, dict):
            chat = chat.as_dict(request.user)

        return Response(chat)


class ChatCreateGroupView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        serializer = CreateGroupChatSerializer(data=request.data)
        serializer.is_valid(raise_exception=True)
        validated_data = serializer.validated_data

        member_ids_str = validated_data['member_ids']
        title = validated_data['title']

        messenger = member_ids_str[0]
        uids = []
        for user_id in member_ids_str.split(","):
            messenger, uid = split_messeger_id(user_id)
            uids.append(uid)

        client = get_chat_client(messenger, request.user)
        chat = client.create_group_chat(uids, title)
        if not isinstance(chat, dict):
            chat = chat.as_dict(request.user)

        return Response(chat)


class SearchChatsView(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        messenger = kwargs['messenger_id']
        client = get_chat_client(messenger, request.user)
        query = request.GET.get("query","")

        if len(query) < 5:
            return Response([])

        return Response(client.search_chats(request.GET.get("query","")))


class ContactsListView(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        messenger = kwargs['messenger_id']
        client = get_chat_client(messenger, request.user)
        except_in_chat_id = int(request.GET.get("except_chat_id","C0")[1:])

        return Response(client.get_contacts(except_in_chat=except_in_chat_id))


class ChatMembersListView(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        messenger, chat_id = split_messeger_id(kwargs['chat_id'])
        client = get_chat_client(messenger, request.user)
        search = request.GET.get('search')
        limit = int(request.GET.get('limit', 30))

        return Response(client.get_chat_members(chat_id, search, limit))


class ForwardMessage(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        serializer = ForwardMessageSerializer(data=request.data)
        serializer.is_valid(raise_exception=True)
        validated_data = serializer.validated_data

        messenger, from_chat_id = split_messeger_id(kwargs['chat_id'])
        msg_id = validated_data['msg_id']
        chat_ids_str = validated_data.get('chat_ids')

        forward_to_chat_ids = []
        copy_to_chat_ids = []
        for chat_id in chat_ids_str.split(","):
            _messenger_id, cid = split_messeger_id(chat_id)
            if _messenger_id == messenger:
                forward_to_chat_ids.append(cid)
            else:
                copy_to_chat_ids.append((_messenger_id, cid))

        client = get_chat_client(messenger, request.user)
        if forward_to_chat_ids:
            client.forward_message(forward_to_chat_ids, from_chat_id, msg_id)
        if copy_to_chat_ids:
            message = client.get_message(from_chat_id, msg_id)
            chat_clients = {}
            for to_messenger_id, to_chat_id in copy_to_chat_ids:
                if to_messenger_id not in chat_clients:
                    chat_clients[to_messenger_id] = get_chat_client(to_messenger_id, request.user)
                chat_client = chat_clients[to_messenger_id]
                chat = chat_client.get_chat(to_chat_id, full_info=False)
                if message['type'] == MESSAGE_TYPE_TEXT:
                    chat_client.send_message_text(chat, message['text'])
                elif message['type'] == MESSAGE_TYPE_PHOTO:
                    parts = message['attachment_image']['url'].split(settings.MEDIA_URL)
                    file = default_storage.open(parts[1])
                    chat_client.send_message_photo(chat, file, message['text'])
                elif message['type'] == MESSAGE_TYPE_FILE:
                    parts = message['attachment_file']['url'].split(settings.MEDIA_URL)
                    file = default_storage.open(parts[1])
                    chat_client.send_message_file(chat, file, message['attachment_file']['name'], message['attachment_file']['file_type'], message['text'])
                else:
                    raise ValidationError("Message of this type can be forwarded to same messenger only!")

        return Response({"result": "ok"})


class ChatView(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        messenger, cid = split_messeger_id(kwargs['chat_id'])

        client = get_chat_client(messenger, request.user)
        chat = client.get_chat(cid)
        if chat and not isinstance(chat, dict):
            chat = chat.as_dict(request.user)

        return Response({"result": chat})


class ChatOpenView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        chat_id = kwargs['chat_id']
        messenger = chat_id[0]
        cid = int(chat_id[1:])
        opened_chat, created = OpenedChat.objects.get_or_create(device=request.auth, defaults={'chat_id': chat_id})
        if not created:
            opened_chat.chat_id = chat_id
            opened_chat.save()
        client = get_chat_client(messenger, request.user)
        client.open_chat(cid)

        return Response({"result": "ok"})

    def get(self, request, *args, **kwargs):
        return self.post(request, *args, **kwargs)


class ChatConversationView(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        is_open_chat = int(request.GET.get('open', 0))
        from_message_id = int(request.GET.get('from_message_id',0))
        is_first_load = int(request.GET.get('first_load',0))
        filter = int(request.GET.get('filter',0))
        limit = DEFAULT_LOAD_MESSAGES_LIMIT
        load_dir = 'o' if from_message_id == 0 else request.GET.get('dir', 'o')
        if load_dir == 'o':
            offset = 0
        else:
            offset = -limit

        if is_first_load and from_message_id:
            offset = -limit
            limit = limit * 2

        chat_id = kwargs['chat_id']
        messenger = chat_id[0]
        cid = int(chat_id[1:])
        client = get_chat_client(messenger, request.user)

        if is_open_chat:
            opened_chat, created = OpenedChat.objects.get_or_create(device=request.auth, defaults={'chat_id': chat_id})
            if not created:
                opened_chat.chat_id = chat_id
                opened_chat.save()
            client.open_chat(cid)


        messages, has_more_older, has_more_newer = client.get_chat_history(cid, from_message_id=from_message_id, limit=limit, filter=filter, offset=offset)

        old_msgs_link = ""
        new_msgs_link = ""
        if messages:
            chat_id = messages[0]['chat_id']
            uri = reverse("api:chat_conversation", kwargs={'chat_id': chat_id})

            if is_first_load:
                if has_more_older:
                    old_msgs_link = build_absolute_uri(uri, request, {
                        'dir': 'o',
                        'from_message_id': messages[-1]['id'],
                    })
                if from_message_id > 0 and has_more_newer:
                    new_msgs_link = build_absolute_uri(uri, request, {
                        'dir': 'n',
                        'from_message_id': messages[0]['id'],
                    })
            else:
                if load_dir == 'o':
                    if has_more_older:
                        old_msgs_link = build_absolute_uri(uri, request, {
                            'dir': 'o',
                            'from_message_id': messages[-1]['id'],
                        })
                else:
                    if has_more_newer:
                        new_msgs_link = build_absolute_uri(uri, request, {
                            'dir': 'n',
                            'from_message_id': messages[0]['id'],
                        })

        return Response({
            'first': is_first_load,
            'dir': load_dir,
            'next': old_msgs_link,
            'prev': new_msgs_link,
            'items': messages
        })


class SearchChatMessagesView(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        query = request.GET.get('query', '')
        from_message_id = int(request.GET.get('from_message_id',0))
        chat_id = kwargs['chat_id']
        filter = int(request.GET.get('filter',0))
        messenger = chat_id[0]
        cid = int(chat_id[1:])
        client = get_chat_client(messenger, request.user)

        messages, has_more_older, has_more_newer = client.search_chat_messages(cid, query, filter, from_message_id=from_message_id, limit=DEFAULT_LOAD_MESSAGES_LIMIT)

        next_link = ''
        prev_link = ''

        if messages:
            chat_id = messages[0]['chat_id']
            uri = reverse("api:chat_conversation", kwargs={'chat_id': chat_id})
            if has_more_older:
                next_link = build_absolute_uri(uri, request, {
                    'dir': 'o',
                    'from_message_id': messages[-1]['id'],
                })
            if has_more_newer:
                prev_link = build_absolute_uri(uri, request, {
                    'dir': 'n',
                    'from_message_id': messages[0]['id'],
                })

        return Response({
            'first': 1,
            'next': next_link,
            'prev': prev_link,
            'items': messages
        })


class MarkSeenMessageView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        me = request.user
        messenger, cid = split_messeger_id(kwargs['chat_id'])
        message_id = int(kwargs['message_id'])

        client = get_chat_client(messenger, me)
        client.mark_seen(cid, message_id)

        return Response({"chat_id": kwargs['chat_id'], "message_id": message_id})

    def get(self, request, *args, **kwargs):
        return self.post(request, *args, **kwargs)


class EditMessageView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        me = request.user
        messenger, chat_id = split_messeger_id(kwargs['chat_id'])
        message_id = int(kwargs['message_id'])
        text = request.POST['text']

        if text:
            client = get_chat_client(messenger, me)
            client.edit_message(chat_id, message_id, text)
        return Response({"result": "ok"})


class SendChatActionView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        me = request.user
        messenger, cid = split_messeger_id(kwargs['chat_id'])
        action = int(request.POST['action'])

        client = get_chat_client(messenger, me)
        client.send_chat_action(cid, action)

        return Response({"result": "ok"})


class DeleteMessagesView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        me = request.user
        messenger, cid = split_messeger_id(kwargs['chat_id'])
        unsend = int(request.POST.get('unsend',0))
        message_ids = [id for id in map(int, request.POST['message_ids'].split(",")) if id > 0]
        if message_ids:
            client = get_chat_client(messenger, me)
            deleted_ids = client.delete_messages(cid, message_ids, unsend)

            return Response({"result": deleted_ids})
        else:
            return Response({"result": []})


class DeleteChatHistoryView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        me = request.user
        messenger, cid = split_messeger_id(kwargs['chat_id'])
        unsend = int(request.POST.get('unsend', 0))
        remove = int(request.POST.get('remove', 0))

        client = get_chat_client(messenger, me)
        client.delete_chat_history(cid, remove, unsend)

        return Response({"result": "ok"})


class LeaveChatView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        me = request.user
        messenger, cid = split_messeger_id(kwargs['chat_id'])

        client = get_chat_client(messenger, me)
        client.leave_chat(cid)

        return Response({"result": "ok"})

class JoinChatView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        me = request.user
        messenger, cid = split_messeger_id(kwargs['chat_id'])

        client = get_chat_client(messenger, me)
        client.join_chat(cid)

        return Response({"result": "ok"})


class AddMembersToChatView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        me = request.user
        messenger, cid = split_messeger_id(kwargs['chat_id'])

        member_ids = [id for id in map(lambda x: int(x[1:]), request.POST['member_ids'].split(","))]

        client = get_chat_client(messenger, me)
        client.add_members_to_chat(cid, member_ids)

        return Response({"result": "ok"})


class SetChatTitle(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        me = request.user
        messenger, cid = split_messeger_id(kwargs['chat_id'])
        title = request.POST['title']

        client = get_chat_client(messenger, me)
        client.set_chat_title(cid, title)

        return Response({"result": "ok"})


class SetChatPhoto(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        me = request.user
        messenger, cid = split_messeger_id(kwargs['chat_id'])
        photo = request.FILES['photo']

        client = get_chat_client(messenger, me)
        client.set_chat_photo(cid, photo)

        return Response({"result": "ok"})


class ChatMessagesExport(APIView):
    renderer_classes = (TemplateHTMLRenderer,)
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        messenger, cid = split_messeger_id(kwargs['chat_id'])
        me = request.user

        client = get_chat_client(messenger, me)
        chat = client.get_chat_as_dict(cid)
        messages = client.get_whole_chat_history_queryset(cid)

        ctx = {
            "constants": constants,
            "chat": chat,
            "messages": messages,
            "me": request.user
        }

        return Response(ctx, template_name="ik/history.html", content_type="text/html")
