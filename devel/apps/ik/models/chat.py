import re
import os
import json
import logging
import subprocess
from io import BytesIO
from PIL import Image
from django.conf import settings
from django.db import models, transaction
from django.db.models import Count
from django.db.models.signals import pre_delete, post_delete
from django.core.files import File
from django.dispatch import receiver
from django.utils.text import Truncator
from django.utils.translation import ugettext_lazy as _
from django.utils.timezone import now
from django_redis import get_redis_connection
from easy_thumbnails.fields import ThumbnailerImageField
from easy_thumbnails.files import ThumbnailerFieldFile, get_thumbnailer
from encrypted_fields.fields import EncryptedTextField
from ik.models import Member
from ik.events import Event
from ik.api.models import AuthorizedDevice
from ik.utils.site import build_absolute_uri
from ik.utils.riscos import riscos_is_image_type
from ik.utils.storage import UUIDPath
from ik.utils.images import image_as_dict
from ik.signals import message_changed, chatmember_created, chatmember_deleted, chat_changed
from ik.api.renderer import json_dumps
from ik.constants import *
logger = logging.getLogger("cc")

#messages_cache_prefix = "m:{member.id}:msg" + settings.CACHE_VERSION_TAG
MAX_MSG_PREVIEW_LENGTH = 120

_default_chat_photo = "default/photo_profile.png"
_default_groupchat_photo = "default/photo_groupchat.png"
_no_pictue_available = "default/no_pictue_available.jpg"

class ChatManager(models.Manager):
    def get_or_create_private_chat(self, me, member):
        if not member.is_active:
            raise ValueError(u"Member %s is deleted, you can't chat with him", member.displayname)

        chat = self.filter(is_group=False, chatmembers__member__in=[me.id, member.id])\
            .annotate(memcnt=Count('chatmembers__member_id')).filter(memcnt=2).first()
        if chat:
            my_chat_mem = chat.get_my_chatmember(me)
            if my_chat_mem.status == CHATMEMBER_STATUS_DELETED:
                my_chat_mem.status = CHATMEMBER_STATUS_NORMAL
                my_chat_mem.hide_msg_upto_time = now()
                my_chat_mem.save()
                chatmember_created.send(sender=chat.__class__, chat=chat, member=me)

            member_chat_mem = chat.get_my_chatmember(member)
            if member_chat_mem.status == CHATMEMBER_STATUS_DELETED:
                member_chat_mem.status = CHATMEMBER_STATUS_NORMAL
                member_chat_mem.hide_msg_upto_time = now()
                member_chat_mem.save()
                chatmember_created.send(sender=chat.__class__, chat=chat, member=member)

            return chat

        # chat = Chat.objects.create(messenger=member1.messenger)
        chat = self.create()
        ChatMember.objects.create(chat=chat, member=me)
        ChatMember.objects.create(chat=chat, member=member)

        chatmember_created.send(sender=Chat, chat=chat, member=me)
        chatmember_created.send(sender=Chat, chat=chat, member=member)
        return chat

    def create_group_chat(self, me, members, title):
        # chat = Chat.objects.create(messenger=messenger, is_group=True)
        chat = self.create(title=title, is_group=True)

        ChatMember.objects.create(chat=chat, member=me, status=CHATMEMBER_STATUS_CREATOR)
        chatmember_created.send(sender=Chat, chat=chat, member=me)

        for member in members:
            if member.is_active:
                ChatMember.objects.create(chat=chat, member=member)
                chatmember_created.send(sender=Chat, chat=chat, member=member)
            else:
                logger.info("Member %s is deleted, you can't create groupchat with him", member.displayname)

        # better join members after new_chat event was sent to member
        #self.send_message_join(chat, [member_ids])
        return chat

class Chat(models.Model):
    id = models.BigAutoField(primary_key=True, unique=True)
    title = models.CharField(max_length=255, default='', blank=True)
    photo_image = ThumbnailerImageField(upload_to=UUIDPath("chat"),
                                        max_length=255, default='', blank=True)
    # messenger = models.SmallIntegerField(choices=MESSENGERS)
    # last_message = models.ForeignKey('Message', related_name='+', blank=True, null=True, on_delete=models.SET_NULL)
    # last_message_time = models.DateTimeField()
    is_group = models.BooleanField(default=False)
    is_common_history = models.BooleanField(default=True)

    objects = ChatManager()

    def get_party_chatmembers(self, me, online=None, limit=20):
        if self.is_group:
            if limit:
                if online is None:
                    return self.chatmembers.select_related('member').filter(member__is_active=True).exclude(member=me).order_by('id')[:limit]
                else:
                    return self.chatmembers.select_related('member').filter(member__online__online=online, member__is_active=True).exclude(member=me).order_by('id')[:limit]
            else:
                if online is None:
                    return self.chatmembers.select_related('member').filter(member__is_active=True).exclude(member=me).order_by('id')
                else:
                    return self.chatmembers.select_related('member').filter(member__online__online=online, member__is_active=True).exclude(member=me).order_by('id')
        else:
            if online is None:
                return self.chatmembers.select_related('member').filter(member__is_active=True).exclude(member=me)
            else:
                return self.chatmembers.select_related('member').filter(member__online__online=online, member__is_active=True).exclude(member=me)

    def get_my_chatmember(self, me):
        return self.chatmembers.get(member_id=me.id)

    def get_photo(self, party_chatmembers):
        if self.is_group:
            if self.photo_image.name != '':
                try:
                    if self.photo_image.width == 0 or self.photo_image.height == 0:
                        logger.warning(u"Chat photo_image [{}] missing or corrupted".format(self.photo_image.name))
                        return ThumbnailerFieldFile(self, self.photo_image, _default_groupchat_photo)
                except Exception as ex:
                    logger.warning(u"Chat photo_image [{}] missing or corrupted. Error: {}".format(self.photo_image.name, ex))
                    return ThumbnailerFieldFile(self, self.photo_image, _default_groupchat_photo)
                return self.photo_image
            else:
                return ThumbnailerFieldFile(self, self.photo_image, _default_groupchat_photo)

        chat_party = party_chatmembers[0]
        return chat_party.member.profile_photo

    def get_pic_small(self, party_chatmembers):
        return build_absolute_uri(self.get_photo(party_chatmembers).get_thumbnail(settings.SMALL_PROFILE_THUMBNAIL_CONF).url)

    def get_title(self, party_chatmembers):
        if self.is_group:
            if self.title:
                return self.title
            else:
                displaynames = [p.member.displayname for p in party_chatmembers]
                title = ", ".join(displaynames)
                if len(party_chatmembers) > 3:
                    title += ", ..."
                return title
        else:
            chat_party = party_chatmembers[0]
            return chat_party.member.displayname

    def get_incoming_seen_message_id(self, my_chatmember):
        if my_chatmember.last_seen_message_id is None:
            return 0
        return my_chatmember.last_seen_message_id

    def get_outgoing_seen_message_id(self, party_chatmembers):
        if self.is_group:
            # find max message_id
            last_seen_message_id = 0
            for p in party_chatmembers:
                if p.last_seen_message_id is not None and p.last_seen_message_id > last_seen_message_id:
                    last_seen_message_id = p.last_seen_message_id
        else:
            last_seen_message_id = party_chatmembers[0].last_seen_message_id
            if last_seen_message_id is None:
                return 0
        return last_seen_message_id

    def get_unread_count(self, me_chatmem):
        last_seen_id = me_chatmem.last_seen_message_id if me_chatmem.last_seen_message_id else 0
        return Message.objects.filter(chat=self, id__gt=last_seen_id).exclude(author_id=me_chatmem.member_id).count()

    def join_chat(self, member):
        if not self.is_group:
            raise ValueError("Chat {} is not group, join impossible".format(self.id))

        defaults = {
            'hide_msg_upto_time': None if self.is_common_history else now()
        }

        chatmember, _created = self.chatmembers.get_or_create(chat=self, member=member, defaults=defaults)
        if _created:
            chatmember_created.send(sender=self.__class__, chat=self, member=member)

        return chatmember, _created

    def leave_chat(self, member):
        chatmem = self.chatmembers.get(chat=self, member=member)
        if self.is_group:
            chatmem.delete()
            chatmem = None
        else:
            chatmem.status = CHATMEMBER_STATUS_DELETED
            chatmem.save()
        chatmember_deleted.send(sender=self.__class__, chat=self, member=member)

        if self.is_group:
            if not self.chatmembers.filter(chat=self).exists():
                logger.info("Group Chat ID:%s is empty, all members leaved. Delete chat.", self.id)
                self.delete()
        else:
            if not self.chatmembers.filter(chat=self).exclude(status=CHATMEMBER_STATUS_DELETED).exists():
                logger.info("Private Chat ID:%s is empty, all members deleted. Delete chat.", self.id)
                self.delete()

        return chatmem

    def set_title(self, title):
        self.title = title
        self.save(update_fields=['title'])
        chat_changed.send(sender=self.__class__, chat=self)

    def set_photo(self, photo_file):
        self.photo_image = photo_file
        self.save()
        chat_changed.send(sender=self.__class__, chat=self)

    def get_last_message(self, me):
        deleted_messages = DeletedMessages.objects.filter(member=me).values_list('message', flat=True)
        return self.messages.exclude(pk__in=deleted_messages).order_by("-id").first()

    def as_dict(self, me):
        my_chatmember = self.get_my_chatmember(me)
        party_chatmembers = self.get_party_chatmembers(me, limit=4)
        if len(party_chatmembers) > 0:
            pic_small = self.get_pic_small(party_chatmembers)
            title = self.get_title(party_chatmembers)
            outgoing_seen_message_id = self.get_outgoing_seen_message_id(party_chatmembers)
        else:
            logger.error("Chat [{}] with one member only!".format(self))
            pic_small = ''
            title = 'No members (empty)'
            outgoing_seen_message_id = 0

        last_msg_obj = self.get_last_message(me)
        if last_msg_obj:
            last_message = last_msg_obj.as_dict(me)
        else:
            last_message = None

        return {
            'id': MESSENGER_CHATCUBE + str(self.id),
            'title': title,
            'pic_small': pic_small,
            'type': CHAT_TYPE_GROUP if self.is_group else CHAT_TYPE_PRIVATE,
            'unread_count': self.get_unread_count(my_chatmember),
            'last_msg': last_message,
            'outgoing_seen_message_id': outgoing_seen_message_id,
            'incoming_seen_message_id': self.get_incoming_seen_message_id(my_chatmember),
            'my_status': my_chatmember.status,
            'members_count': self.chatmembers.count() - 1 if self.is_group else 1,
            'member': None if self.is_group else party_chatmembers[0].member.as_dict(me)
        }

    def __str__(self):
        return u"Chat:{} Group:{}".format(self.id, self.is_group)

    class Meta:
        db_table = 'chats'


class ChatMember(models.Model):
    id = models.BigAutoField(primary_key=True)
    chat = models.ForeignKey(Chat, related_name='chatmembers', on_delete=models.CASCADE)
    member = models.ForeignKey(Member, related_name='chats', on_delete=models.CASCADE)
    last_seen_message = models.ForeignKey('Message', blank=True, null=True, related_name='+', on_delete=models.SET_NULL)

    # field used when member deleted the chat history but then he joined again
    # then member will see only messages after this time
    # but others will see own set of messages or whole history
    # this filed can be also used in group chats when members must see only the history after join
    # but not all previous history
    hide_msg_upto_time = models.DateTimeField(blank=True, null=True)

    status = models.SmallIntegerField(default=CHATMEMBER_STATUS_NORMAL)

    def as_dict(self, me):
        return {
            'chat_id': MESSENGER_CHATCUBE + str(self.chat_id),
            'status': self.status,
            'member': self.member.as_dict(me)
        }

    def can_write(self):
        return self.status <= CHATMEMBER_STATUS_NORMAL

    def __str__(self):
        return u"ChatMember:{} Chat:{} Member:{}".format(self.id, self.chat_id, self.member)

    class Meta:
        db_table = 'chat_members'
        unique_together = ('chat', 'member')


class OpenedChat(models.Model):
    device = models.OneToOneField(AuthorizedDevice, primary_key=True, related_name='opened_chat', on_delete=models.CASCADE)
    chat_id = models.CharField(max_length=65, db_index=True)

    class Meta:
        db_table = 'chat_opened'


# class MessageManager(models.Manager):
#     def _final_save_message(self, newmsg, author, chat, type, reply_to_id, attachment=None):
#         if reply_to_id:
#             newmsg.reply_to = self.filter(pk=reply_to_id, chat=chat).first()
#
#         newmsg.author = author
#         newmsg.chat = chat
#         newmsg.type = type
#         newmsg.sendtime = now()
#
#         if attachment:
#             with transaction.atomic():
#                 newmsg.save()
#                 attachment.message = newmsg
#                 attachment.save()
#         else:
#             newmsg.save()
#
#         after_send_message.send(sender=Message, message=newmsg)
#         return newmsg
#
#     def send_message_text(self, author: Member, chat: Chat, text, reply_to_id=None):
#         msg = Message(text=text)
#         return self._final_save_message(msg, author, chat, MESSAGE_TYPE_TEXT, reply_to_id)
#
#     def send_message_file(self, author: Member, chat: Chat, file, file_type=None, caption=None, reply_to_id=None):
#         msg = Message()
#         if not caption:
#             msg.text = caption
#         return self._final_save_message(msg, author, chat, MESSAGE_TYPE_FILE, reply_to_id, AttachmentFile(file=file, file_type=file_type))
#
#     def send_message_photo(self, author: Member, chat: Chat, file, caption=None, reply_to_id=None):
#         msg = Message()
#         if not caption:
#             msg.text = caption
#         return self._final_save_message(msg, author, chat, MESSAGE_TYPE_PHOTO, reply_to_id, AttachmentPhoto(file=file))
#
#     def send_message_sticker(self, author: Member, chat: Chat, sticker, reply_to_id=None):
#         msg = Message(text=sticker)
#         return self._final_save_message(msg, author, chat, MESSAGE_TYPE_STICKER, reply_to_id)
#
#     def send_message_join(self, author: Member, chat: Chat, member_ids):
#         msg = Message(text=json_dumps(member_ids))
#         return self._final_save_message(msg, author, chat, MESSAGE_TYPE_JOIN, None)
#
#     def send_message_leave(self, author: Member, chat: Chat):
#         msg = Message()
#         return self._final_save_message(msg, author, chat, MESSAGE_TYPE_LEAVE, None)
#
#     def forward_message(self, author: Member, chat: Chat, orig_message):
#         newmsg = Message()
#         newmsg.text = orig_message.text
#         if orig_message.type == MESSAGE_TYPE_FILE:
#             attachment = AttachmentFile(file=orig_message.attachment_file.file, file_type=orig_message.attachment_file.file_type)
#         elif orig_message.type == MESSAGE_TYPE_PHOTO:
#             attachment = AttachmentPhoto(file=orig_message.attachment_file.file)
#         else:
#             attachment = None
#         return self._final_save_message(newmsg, author, chat, orig_message.type, 0, attachment)

url_match_re = re.compile(r"(?i)\b((?:https?://|www\d{0,3}[.]|[a-z0-9.\-]+[.][a-z]{2,4}/)(?:[^\s()<>]+|\(([^\s()<>]+|(\([^\s()<>]+\)))*\))+(?:\(([^\s()<>]+|(\([^\s()<>]+\)))*\)|[^\s`!()\[\]{};:'\".,<>?«»“”‘’]))")
email_match_re = re.compile(r"([a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+)")

class Message(models.Model):
    SYSTEM_MESSAGES = (MESSAGE_TYPE_LEAVE, MESSAGE_TYPE_JOIN, MESSAGE_TYPE_CALL)

    id = models.BigAutoField(primary_key=True)
    type = models.SmallIntegerField(verbose_name='Message type', choices=MESSAGE_TYPES)
    reply_to = models.ForeignKey('Message', default=None, blank=True, null=True, related_name='replies', on_delete=models.SET_NULL)
    author = models.ForeignKey(Member, related_name="my_messages", on_delete=models.CASCADE, blank=True, null=True)
    chat = models.ForeignKey(Chat, related_name='messages', on_delete=models.CASCADE)
    text = EncryptedTextField(blank=True, null=True)

    forwarded_from = models.ForeignKey('Message', default=None, blank=True, null=True, related_name='+',
                                 on_delete=models.SET_NULL)

    sendtime = models.DateTimeField(auto_now_add=True, verbose_name=_("Send time"))
    changedtime = models.DateTimeField(null=True, blank=True)
    deleted = models.BooleanField(default=False)

    # objects = MessageManager()

    def update_entities(self):
        if self.type == MESSAGE_TYPE_TEXT:
            self.entities.all().delete()

            text = self.text
            urls = url_match_re.findall(text)
            for u in urls:
                url = u[0]
                start = text.index(url)
                length = len(url)
                text = text.replace(url, "X"*length, 1)
                self.entities.create(type=TEXT_ENTITY_URL, start=start, len=length)

            for email in email_match_re.findall(text):
                start = text.index(email)
                length = len(email)
                text = text.replace(email, "X"*length, 1)
                self.entities.create(type=TEXT_ENTITY_EMAIL, start=start, len=length)


    def change_message(self, message=None):
        self.text = message
        self.changedtime = now()
        self.save()
        self.update_entities()

        message_changed.send(sender=Message, message=self)

    def set_seen(self, seen_by_member):
        if self.author_id != seen_by_member.id:
            redis_conn = get_redis_connection()
            redis_lck_key = "ik:msg:seen-{}-{}".format(self.id, seen_by_member.id)
            if redis_conn.set(redis_lck_key, "1", ex=3, nx=True): # set key expired after 3 sec to avoid same multiple requests
                chatmember = ChatMember.objects.select_related('chat').filter(chat=self.chat, member=seen_by_member).first()
                if chatmember and chatmember.last_seen_message_id != self.id:
                    chatmember.last_seen_message = self
                    chatmember.save(update_fields=['last_seen_message'])

                    chat_id = MESSENGER_CHATCUBE + str(self.chat_id)
                    if self.author.online.online:
                        Event.send(self.author, Event.EV_CHAT_UPDATED_OUTBOX, {'id': chat_id, 'outgoing_seen_message_id': self.id})

                    Event.send(seen_by_member, Event.EV_CHAT_UPDATED,
                                   {'id': chat_id,
                                    'incoming_seen_message_id': self.id,
                                    'unread_count': chatmember.chat.get_unread_count(chatmember)
                                    })

    def message_preview(self):
        msg = Truncator(self.message_text()).words(5, "...")
        if len(msg) > MAX_MSG_PREVIEW_LENGTH:
            return Truncator(msg).chars(MAX_MSG_PREVIEW_LENGTH, "...")
        else:
            return msg

    def message_text(self):
        if self.deleted:
            return _("Message deleted")

        elif self.type == MESSAGE_TYPE_JOIN:
            memids = json.loads(self.text)
            names = []
            for mid in memids:
                mem = Member.objects.filter(pk=mid).first()
                if mem:
                    names.append(mem.displayname)

            return _("{} joined chat").format(", ".join(names))

        elif self.type == MESSAGE_TYPE_LEAVE:
            return _("{} left chat").format(self.author.displayname)

        elif self.text:
            return self.text

        elif self.type == MESSAGE_TYPE_TEXT:
            return _('Empty message')
        else:
            return self.get_type_display()

    def get_flags(self, me):
        flags = MESSAGE_FLAG_CAN_BE_EDITED

        if me.id == self.author_id:
            flags = flags | MESSAGE_FLAG_OUTGOING

        if self.type in self.SYSTEM_MESSAGES:
            flags = flags | MESSAGE_FLAG_SYSTEM

        if self.deleted:
            flags = flags | MESSAGE_FLAG_DELETED

        return flags

    def _fill_message_content(self, result):
        try:
            if self.type == MESSAGE_TYPE_PHOTO:
                result['attachment_image'] = self.attachment_photo.as_dict()
            elif self.type == MESSAGE_TYPE_FILE:
                result['attachment_file'] = self.attachment_file.as_dict()
                result['text'] = result['attachment_file']['name'] if result['attachment_file']['name'] else "Uploaded file"
            elif self.type == MESSAGE_TYPE_STICKER:
                result['text'] = ''
                result['attachment_image'] = image_as_dict(self.text, _no_pictue_available)
            elif self.type == MESSAGE_TYPE_TEXT:
                entities_qs = self.entities.all().order_by('start')
                result['entities'] = [e.as_dict() for e in entities_qs]

        except models.ObjectDoesNotExist:
            logger.error("Attachment does not exists for message id:{} type:{}".format(self.id, self.type))

    def get_sticker_dict(self):
        return image_as_dict(self.text, _no_pictue_available)

    def as_reply_dict(self):
        result = {
            'id': self.id,
            'type': self.type,
            'author_id': MESSENGER_CHATCUBE + str(self.author_id),
            'text': self.message_text(),
        }
        self._fill_message_content(result)
        return result

    def as_dict(self, me, with_reply_info=False):
        result = {
            'id': self.id,
            'type': self.type,
            'flags': self.get_flags(me),
            'author_id': MESSENGER_CHATCUBE+str(self.author_id),
            'chat_id': MESSENGER_CHATCUBE+str(self.chat_id),
            'text': self.message_text(),
            'sendtime': self.sendtime,
            'changedtime': self.changedtime if self.changedtime else 0,
        }

        self._fill_message_content(result)

        if self.forwarded_from:
            forwarded_chat = self.forwarded_from.chat
            party_chatmembers = forwarded_chat.get_party_chatmembers(me, limit=4)
            if len(party_chatmembers) > 0:
                title = forwarded_chat.get_title(party_chatmembers)
                if forwarded_chat.is_group:
                    forw_chat_id = MESSENGER_CHATCUBE + str(forwarded_chat.id)
                    forw_user_id = ''
                else:
                    forw_user_id = MESSENGER_CHATCUBE + str(party_chatmembers[0].member_id)
                    forw_chat_id = ''
            else:
                title = 'No members (empty)'
                forw_chat_id = ''
                forw_user_id = ''

            result['forward_info'] = {
                'title': title,
                'chat_id': forw_chat_id,
                'user_id': forw_user_id
            }

        if with_reply_info and self.reply_to:
            result['reply_info'] = self.reply_to.as_reply_dict()

        return result

    def __str__(self):
        return u"Message id:{} [{}] \"{}\"".format(self.id,
                                                   self.author,
                                                   self.text)

    class Meta:
        db_table = 'chat_messages'


class ChatMessageEntities(models.Model):
    message = models.ForeignKey(Message, related_name='entities', on_delete=models.CASCADE)
    type = models.IntegerField()
    start = models.IntegerField()
    len =  models.IntegerField()
    value = models.CharField(max_length=255, default='')

    def as_dict(self):
        return {
            's': self.start,
            'l': self.len,
            't': self.type,
            'v': self.value
        }

    class Meta:
        db_table = 'chat_messages_entities'


_generate_attachment_file_name = UUIDPath("attachment")
_image_type_exts = {
    "tiff", "tif", "jpeg", "jpg", "png"
}

class AttachmentFile(models.Model):
    message = models.OneToOneField(Message, related_name='attachment_file', on_delete=models.CASCADE, primary_key=True)
    name = models.CharField(max_length=255, default='')
    file = models.FileField(max_length=255,
                                  upload_to=_generate_attachment_file_name,
                                  default=None)
    file_type = models.IntegerField(default=0)

    def is_image(self):
        try:
            try:
                if riscos_is_image_type(int(self.file_type)):
                    return True
            except:
                pass
            ext = self.file.name.split(".")[-1]
            if ext in _image_type_exts:
                return True
        except:
            pass
        return False

    def as_dict(self):
        result = {
            'url': build_absolute_uri(self.file.url),
            'name': self.name,
            'file_type': self.file_type,
            'is_image': self.is_image(),
            'size': self.file.size,
        }
        if self.file_type == 0xff9:
            jpg_file = self.file.path + ".jpg"
            if not os.path.exists(jpg_file):
                spr2png = "{} {} | /usr/bin/convert - {}".format(
                    os.path.join(os.path.dirname(settings.PROJECT_DIR), "bin", "spr2ras"),
                    self.file.path,
                    jpg_file)

                # print("spr2png=",spr2png)
                process = subprocess.run(spr2png, shell=True)
                if process.returncode != 0:
                    logger.error("Failed to convert sprite [%s] to .jpg", self.file.path)
                    f = open(jpg_file,"wb")
                    f.close()

            if os.path.getsize(jpg_file) > 16:
                img = image_as_dict(self.file.name + ".jpg", _no_pictue_available)
                if img:
                    result['is_image'] = True
                    result['thumb_url'] = img['thumb_url']
                    result['thumb_width'] = img['thumb_width']
                    result['thumb_height'] = img['thumb_height']
                else:
                    logger.error("Can't make thumbnail for [%s]", jpg_file)


        return result
    def __str__(self):
        return u"{}:{}".format(self.id, self.file.name)

    class Meta:
        db_table = "attachments_file"


class AttachmentPhoto(models.Model):
    message = models.OneToOneField(Message, related_name='attachment_photo', on_delete=models.CASCADE, primary_key=True)
    file = models.ImageField(max_length=255,
                                  upload_to=_generate_attachment_file_name,
                                  default=None)


    def as_dict(self):
        return image_as_dict(self.file.name, _no_pictue_available)

    def __str__(self):
        return u"{}:{}".format(self.id, self.file.name)

    class Meta:
        db_table = "attachments_photo"



class DeletedMessages(models.Model):
    member = models.ForeignKey(Member, on_delete=models.CASCADE)
    message = models.ForeignKey(Message, on_delete=models.CASCADE)

    class Meta:
        unique_together = ('member', 'message')
        db_table = "messages_deleted"


def get_common_online_private_members(me):
    chats = Chat.objects.filter(chatmembers__member=me, is_group=False).values_list('id', flat=True)
    member_ids =  ChatMember.objects.exclude(member=me).filter(chat__in=chats, status__lt=CHATMEMBER_STATUS_BANNED).values_list('member_id', flat=True)
    return Member.objects.filter(id__in=member_ids, online__online=True)


# delete private chats when member deleted
def on_pre_delete_chatmember(sender, instance=None, **kwargs):
    Chat.objects.filter(is_group=False, chatmembers__member__in=[instance.id]).delete()

pre_delete.connect(on_pre_delete_chatmember, sender=Member)


def on_delete_auto_delete_file(sender, instance, **kwargs):
    """
    Deletes file from filesystem
    when corresponding `MediaFile` object is deleted.
    """
    if instance.file:
        if os.path.isfile(instance.file.path):
            os.remove(instance.file.path)

post_delete.connect(on_delete_auto_delete_file, sender=AttachmentPhoto)
post_delete.connect(on_delete_auto_delete_file, sender=AttachmentFile)
