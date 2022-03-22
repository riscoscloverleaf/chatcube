import logging
import json
from django.db import transaction
from django.db.models import Q
from django.conf import settings
from django.utils.timezone import now
from rest_framework.exceptions import ValidationError, PermissionDenied

from ik.constants import *
from ik.events import Event
from ik.models.chat import Chat, ChatMember, Message, AttachmentFile, AttachmentPhoto, DeletedMessages
from ik.models.members import Member
from ik.signals import after_send_message, chat_action, messages_deleted, chat_history_cleared
from ..client import IKAbstractChatClient, register_chat_client

logger = logging.getLogger("cc")

class IKChatcubeClient(IKAbstractChatClient):
    @staticmethod
    def get_client(me, raise_exception=True):
        if not hasattr(me, "_cc_client"):
            me._cc_client = IKChatcubeClient(me)

        return me._cc_client

    def get_or_create_private_chat(self, member_id, full_info=True):
        mem = Member.objects.filter(pk=member_id, is_active=True).first()
        if not mem:
            logger.error("Recipient {} not found".format(member_id))
            raise ValidationError("Recipient {} not found".format(member_id))

        return Chat.objects.get_or_create_private_chat(self.me, mem)

    def create_group_chat(self, member_ids, title):
        members = []
        for memid in member_ids:
            mem = Member.objects.filter(pk=memid, is_active=True).first()
            if not mem:
                logger.error("Recipient {} not found".format(memid))
                raise ValidationError("Recipient {} not found".format(memid))
            members.append(mem)

        return Chat.objects.create_group_chat(self.me, members, title)

    def get_user_as_dict(self, user_id, raise_not_found_exception=True):
        member = Member.objects.select_related('online').filter(pk=user_id, is_active=True).first()
        if not member:
            raise ValidationError("Member {} not found".format(user_id))
        return member.as_dict(self.me)

    def get_users_as_dict(self, user_ids, raise_not_found_exception=True):
        if raise_not_found_exception:
            return [self.get_user_as_dict(uid, raise_not_found_exception=True) for uid in user_ids]
        else:
            members = Member.objects.select_related('online').filter(pk__in=user_ids, is_active=True)
            return [mem.as_dict(self.me) for mem in members]

    def get_chat(self, chat_id, full_info=True) -> Chat:
        chat = Chat.objects.filter(pk=chat_id).first()
        if not chat:
            raise ValidationError("Chat {} not found".format(chat_id))
        return chat

    def get_chat_as_dict(self, chat_id):
        return self.get_chat(chat_id).as_dict(self.me)

    def get_chats_as_dict(self):
        chats = []
        chats_qs = Chat.objects.filter(chatmembers__member=self.me, chatmembers__status__lt=CHATMEMBER_STATUS_DELETED)
        for chat in chats_qs:
            chats.append(chat.as_dict(self.me))
        return chats

    def get_chat_members(self, chat_id, search, limit):
        chatmembers_qs = ChatMember.objects.select_related('member').filter(chat=chat_id, member__is_active=True).exclude(member_id=self.me.id)
        if search:
            chatmembers_qs = chatmembers_qs.filter(member__displayname__icontains=search)
        return [cmem.as_dict(self.me) for cmem in chatmembers_qs] # ignore limit (should be fixed when implemented partial download in client)

    def get_message(self, chat_id, message_id):
        chatMember = ChatMember.objects.filter(chat=chat_id, member=self.me).first()
        if not chatMember:
            msg = "Member {} not in the chat_id={}".format(self.me, chat_id)
            logger.error(msg)
            raise PermissionDenied(msg, 'chat:member_not_in_chat')

        if chatMember.status == CHATMEMBER_STATUS_DELETED:
            msg = "Member {} deleted from chat_id={} ()".format(self.me, chat_id)
            logger.error(msg)
            raise PermissionDenied(msg, 'chat:member_deleted')

        if chatMember.status == CHATMEMBER_STATUS_BANNED:
            msg = "Member {} banned from the chat_id={}".format(self.me, chat_id)
            logger.error(msg)
            raise PermissionDenied(msg, 'chat:member_banned')

        return Message.objects.get(pk=message_id, deleted=False).as_dict(self.me)

    def get_chat_history(self, chat_id, from_message_id=0, limit=20, filter=None, offset=0):
        chatMember = ChatMember.objects.filter(chat=chat_id, member=self.me).first()
        if not chatMember:
            msg = "Member {} not in the chat_id={}".format(self.me, chat_id)
            logger.error(msg)
            raise PermissionDenied(msg, 'chat:member_not_in_chat')

        if chatMember.status == CHATMEMBER_STATUS_DELETED:
            msg = "Member {} deleted from chat_id={} ()".format(self.me, chat_id)
            logger.error(msg)
            raise PermissionDenied(msg, 'chat:member_deleted')

        if chatMember.status == CHATMEMBER_STATUS_BANNED:
            msg = "Member {} banned from the chat_id={}".format(self.me, chat_id)
            logger.error(msg)
            raise PermissionDenied(msg, 'chat:member_banned')

        qs = Message.objects.filter(chat=chat_id, deleted=False)
        if chatMember.hide_msg_upto_time:
            qs = qs.filter(sendtime__gt=chatMember.hide_msg_upto_time)

        if filter == self.MESSAGES_FILTER_ATTACHMENTS:
            qs = qs.filter(Q(attachment_file__file__isnull=False) | Q(attachment_photo__file__isnull=False))
        elif filter == self.MESSAGES_FILTER_LINKS:
            qs = qs.filter(entities__type__in=(TEXT_ENTITY_URL,TEXT_ENTITY_TEXT_URL))
        elif filter == self.MESSAGES_FILTER_EMAILS:
            qs = qs.filter(entities__type=TEXT_ENTITY_EMAIL)

        deleted_messages = DeletedMessages.objects.filter(member=self.me).values_list('message', flat=True)
        qs = qs.exclude(pk__in=deleted_messages)

        has_more_newer = False
        has_more_older = False
        if from_message_id:
            if offset < 0:
                lim_newer = -offset+1
                msgs_list = list(qs.filter(id__gte=from_message_id).order_by("id")[:lim_newer])
                has_more_newer = len(msgs_list) == lim_newer
                print("has_more_newer=", has_more_newer, " len=", len(msgs_list))
                if has_more_newer:
                    msgs_list.pop()

                msgs_list.reverse()
                lim_older = limit + offset # offset is negative here
                if lim_older > 0:
                    lim_older += 1
                    older_list = list(qs.filter(id__lt=from_message_id).order_by("-id")[:lim_older])
                    has_more_older = len(older_list) == lim_older
                    print("has_more_older=", has_more_older, " len=", len(older_list))
                    if has_more_older:
                        older_list.pop()
                    msgs_list = msgs_list + older_list
            else:
                limit += 1
                msgs_list = list(qs.filter(id__lt=from_message_id).order_by("-id")[:limit])
                has_more_older = len(msgs_list) == limit
                if has_more_older:
                    msgs_list.pop()
        else:
            limit += 1
            msgs_list = list(qs.order_by("-id")[:limit])
            has_more_older = len(msgs_list) == limit
            if has_more_older:
                msgs_list.pop()

        # if not from_message_id \
        #     and chatMember.last_seen_message_id \
        #     and len(qs_final) == limit \
        #     and qs_final[-1].id > chatMember.last_seen_message_id:
        #         qs_final = qs.filter(id__gte=chatMember.last_seen_message_id).order_by("-sendtime")[:100]

        messages = []
        replies = {}
        for msg in msgs_list:
            msg_dict = msg.as_dict(self.me)
            if msg.reply_to:
                if msg.reply_to_id not in replies:
                    replies[msg.reply_to_id] = [msg_dict]
                else:
                    replies[msg.reply_to_id].append(msg_dict)
            messages.append(msg_dict)

        if replies:
            for rmsg in Message.objects.filter(id__in=list(replies.keys())):
                for msg_dict in replies[rmsg.id]:
                    msg_dict['reply_info'] = rmsg.as_reply_dict()

        return messages, has_more_older, has_more_newer

    def get_whole_chat_history_queryset(self, chat_id):
        chatMember = ChatMember.objects.filter(chat=chat_id, member=self.me).first()
        if not chatMember:
            msg = "Member {} not in the chat_id={}".format(self.me, chat_id)
            logger.error(msg)
            raise PermissionDenied(msg, 'chat:member_not_in_chat')

        if chatMember.status == CHATMEMBER_STATUS_DELETED:
            msg = "Member {} deleted from chat_id={} ()".format(self.me, chat_id)
            logger.error(msg)
            raise PermissionDenied(msg, 'chat:member_deleted')

        if chatMember.status == CHATMEMBER_STATUS_BANNED:
            msg = "Member {} banned from the chat_id={}".format(self.me, chat_id)
            logger.error(msg)
            raise PermissionDenied(msg, 'chat:member_banned')

        qs = Message.objects.filter(chat=chat_id, deleted=False)
        if chatMember.hide_msg_upto_time:
            qs = qs.filter(sendtime__gt=chatMember.hide_msg_upto_time)

        deleted_messages = DeletedMessages.objects.filter(member=self.me).values_list('message', flat=True)
        qs = qs.exclude(pk__in=deleted_messages)

        messages = []
        authors = {}
        for msg in qs:
            msg_dict = msg.as_dict(self.me)
            messages.append(msg_dict)

            author_id = msg_dict['author_id'][1:]
            if author_id not in authors:
                authors[author_id] = []
            authors[author_id].append(msg_dict)

        for author in self.get_users_as_dict(authors.keys(), raise_not_found_exception=False):
            for msg in authors[author['id'][1:]]:
                msg['author'] = author

        return messages

    def mark_seen(self, chat_id, message_id):
        msg = Message.objects.filter(id=message_id, chat_id=chat_id).first()
        if msg:
            msg.set_seen(self.me)

    def check_send_permissions(self, chat: Chat) -> None:
        my_chatmember = chat.get_my_chatmember(self.me)
        if not my_chatmember.can_write():
            logger.warning(
                "Member [{}] can't send to chat [{}] status={}".format(self.me, chat, my_chatmember.status))
            raise ValidationError("You can't send message in this chat")

    def _final_save_message(self, newmsg, chat, type, reply_to_id, attachment=None):
        if reply_to_id:
            newmsg.reply_to = Message.objects.filter(pk=reply_to_id, chat=chat).first()

        newmsg.author = self.me
        newmsg.chat = chat
        newmsg.type = type
        newmsg.sendtime = now()

        if attachment:
            with transaction.atomic():
                newmsg.save()
                attachment.message = newmsg
                attachment.save()
        else:
            newmsg.save()

        newmsg.update_entities()

        after_send_message.send(sender=Message, message=newmsg)
        return newmsg.as_dict(self.me, with_reply_info=True)

    def send_message_text(self, chat: Chat, text, reply_to_id=None):
        msg = Message(text=text)
        return self._final_save_message(msg, chat, MESSAGE_TYPE_TEXT, reply_to_id)

    def send_message_file(self, chat: Chat, file, file_name=None, file_type=None, caption=None, reply_to_id=None):
        msg = Message()
        if not caption:
            msg.text = caption

        if not file_name:
            file_name = file.name

        return self._final_save_message(msg, chat, MESSAGE_TYPE_FILE, reply_to_id, AttachmentFile(file=file, name=file_name, file_type=file_type))

    def send_message_photo(self, chat: Chat, file, caption=None, reply_to_id=None):
        msg = Message()
        if not caption:
            msg.text = caption
        return self._final_save_message(msg, chat, MESSAGE_TYPE_PHOTO, reply_to_id, AttachmentPhoto(file=file))

    def send_message_sticker(self, chat: Chat, sticker, reply_to_id=None):
        msg = Message(text=sticker)
        return self._final_save_message(msg, chat, MESSAGE_TYPE_STICKER, reply_to_id)

    def send_message_join(self, chat: Chat, member_ids):
        msg = Message(text=json.dumps(member_ids))
        return self._final_save_message(msg, chat, MESSAGE_TYPE_JOIN, None)

    def send_message_leave(self, chat: Chat):
        msg = Message()
        return self._final_save_message(msg, chat, MESSAGE_TYPE_LEAVE, None)

    def send_chat_action(self, chat_id: str, action: int):
        chat_action.send(Event, member=self.me, chat_id=chat_id, action=action)

    def forward_message(self, to_chat_ids, from_chat_id, message_id):
        orig_message = Message.objects.get(pk=message_id)

        for chat in Chat.objects.filter(id__in=to_chat_ids):
            newmsg = Message(text=orig_message.text, forwarded_from=orig_message)
            if orig_message.type == MESSAGE_TYPE_FILE:
                attachment = AttachmentFile(name=orig_message.attachment_file.name,
                                            file=orig_message.attachment_file.file.file,
                                            file_type=orig_message.attachment_file.file_type)
            elif orig_message.type == MESSAGE_TYPE_PHOTO:
                attachment = AttachmentPhoto(file=orig_message.attachment_photo.file.file)
            else:
                attachment = None
            self._final_save_message(newmsg, chat, orig_message.type, 0, attachment)

    def delete_messages(self, chat_id, message_ids: list, unsend):
        chat = Chat.objects.get(pk=chat_id)
        if unsend:
            ids = list(Message.objects.filter(pk__in=message_ids, author=self.me, chat=chat).values_list('id', flat=True))
            Message.objects.filter(id__in=ids).delete()
        else:
            ids = list(Message.objects.filter(pk__in=message_ids).values_list('id', flat=True))
            for mid in ids:
                try:
                    DeletedMessages.objects.create(member=self.me, message_id=mid)
                except:
                    pass
        messages_deleted.send(sender=Message, me=self.me, chat=chat, message_ids=ids, unsend=unsend)

    def delete_chat_history(self, chat_id, remove_from_chat_list, unsend):
        chat = Chat.objects.get(pk=chat_id)

        if remove_from_chat_list:
            self.leave_chat(chat_id)

            if not chat.is_group and unsend:
                for chatmem in chat.get_party_chatmembers(self.me):
                    chat.leave_chat(chatmem.member)
        else:
            if not chat.is_group and unsend:
                Message.objects.filter(chat=chat).delete()
            else:
                chatmem = ChatMember.objects.get(member=self.me, chat=chat)
                chatmem.hide_msg_upto_time = now()
                chatmem.save()

            chat_history_cleared.send(sender=Message, me=self.me, chat=chat, unsend=unsend)


    def leave_chat(self, chat_id):
        chat = Chat.objects.get(pk=chat_id)
        if chat.is_group and chat.id not in settings.PUBLIC_CHATCUBE_GROUPS:
            self.send_message_leave(chat)
        chat.leave_chat(self.me)

    def get_contacts(self, except_in_chat=None):
        qs = Member.objects.filter(is_staff=False, is_active=True).exclude(id=self.me.id)
        if except_in_chat:
            qs = qs.exclude(chats__chat_id=except_in_chat)
        result = []
        for mobj in qs:
            result.append(mobj.as_dict(self.me))
        return result

    def set_chat_title(self, chat_id, title):
        chat = Chat.objects.get(pk=chat_id)
        if chat.is_group:
            chat.set_title(title)

    def set_chat_photo(self, chat_id, photo_file):
        chat = Chat.objects.get(pk=chat_id)
        if chat.is_group:
            chat.set_photo(photo_file)

    def add_members_to_chat(self, chat_id, member_ids):
        chat = Chat.objects.get(pk=chat_id)
        if chat.is_group:
            memids = []
            for mem in Member.objects.filter(pk__in=member_ids, is_active=True):
                chat.join_chat(mem)
                memids.append(mem.id)

            self.send_message_join(chat, memids)

    def join_chat(self, chat_id):
        chat = Chat.objects.get(pk=chat_id)
        if chat.is_group:
            chat.join_chat(self.me)

            self.send_message_join(chat, [self.me.pk])

    def edit_message(self, chat_id, message_id, text):
        msg = Message.objects.get(pk=message_id)
        msg.change_message(text)

    def search_chat_messages(self, chat_id, query, filter, from_message_id=0, limit=20, sender_id=None):
        qs = Message.objects.filter(chat=chat_id, deleted=False)

        if filter == self.MESSAGES_FILTER_ATTACHMENTS:
            qs = qs.filter(Q(attachment_file__file__isnull=False) | Q(attachment_photo__file__isnull=False))
        elif filter == self.MESSAGES_FILTER_LINKS:
            qs = qs.filter(entities__type__in=(TEXT_ENTITY_URL,TEXT_ENTITY_TEXT_URL))
        elif filter == self.MESSAGES_FILTER_EMAILS:
            qs = qs.filter(entities__type=TEXT_ENTITY_EMAIL)

        if from_message_id:
            qs = qs.filter(id__lt=from_message_id)
        if sender_id:
            qs = qs.filter(author__id=sender_id)

        deleted_messages = DeletedMessages.objects.filter(member=self.me).values_list('message', flat=True)

        qs = qs.exclude(pk__in=deleted_messages).order_by("-id").values_list("id", "text")
        found_msg_id = None
        if query:
            query = query.lower()
            for msg_id, msg_text in qs:
                if msg_text and query in msg_text.lower():
                    print("found message = ", msg_text, " id=", msg_id)
                    found_msg_id = msg_id
                    break
        else:
            found_msg = qs.first()
            if found_msg:
                found_msg_id = found_msg[0]
                print("found message = ", found_msg)

        if not found_msg_id:
            return [], False, False
        return self.get_chat_history(chat_id, found_msg_id, limit=limit*2, offset=-limit)

def get_chatcube_client(member):
    if not hasattr(member, "_cc_client"):
        member._cc_client = IKChatcubeClient(member)

    return member._cc_client

register_chat_client(MESSENGER_CHATCUBE, IKChatcubeClient)