import os
import time
import logging
from typing import Any, Dict, List, Type, Callable, Optional, DefaultDict, Tuple
from pathlib import PosixPath
from django.conf import settings
from django.db import models
from django.core.files.storage import default_storage
from django.utils.translation import ugettext as _
from django.urls import reverse
from PIL import Image
from rest_framework.exceptions import APIException, NotFound, ValidationError
from ik.utils.storage import MD5PathWithExt, MD5PathWithBasename
from ik.utils.site import build_absolute_uri
from ik.utils.riscos import mime_to_riscos_type, riscos_is_image_type
from ik.constants import *
from ik.events import Event
from easy_thumbnails.files import get_thumbnailer
from ik.models.members import _default_profile_photo
from .base import IKTelegramBase
from .models import TelegramAccount
from ..client import IKAbstractChatClient, register_chat_client
logger = logging.getLogger('ik')

TELEGRAM2CHAT_ACTIONS = {
    'chatActionCancel': CHAT_ACTION_CANCEL,
    'chatActionTyping': CHAT_ACTION_TYPING
}

CHAT2TELEGRAM_ACTIONS = {
    CHAT_ACTION_CANCEL: 'chatActionCancel',
    CHAT_ACTION_TYPING: 'chatActionTyping'
}

file_name_generators = {
    'photos': MD5PathWithExt("telegram/photos"),
    'videos': MD5PathWithExt("telegram/videos")
}

photos_name_generator = MD5PathWithExt("telegram/photos")
videos_name_generator = MD5PathWithBasename("telegram/videos")
stickers_name_generator = MD5PathWithExt("telegram/stickers")
attachments_name_generator = MD5PathWithBasename("telegram/attachments")

name_generators = {
    'photos': photos_name_generator,
    'videos': videos_name_generator,
    'stickers': stickers_name_generator,
    'attachments': attachments_name_generator,
}

class TelegramException(APIException):
    def __init__(self, message, code, addition_code=""):
        if addition_code == "" and ' ' not in message:
            addition_code = message
        super().__init__("Telegram: {}, code: {}".format(message, code), "telegram:{}:{}".format(code, addition_code))
        self.code = code

class TelegramUnauthorizedException(TelegramException):
    pass

class TelegramPhotoConverter:
    preferred_sizes = ('y','w','x','m','s','i') # 1280x1280, 800x800, 2560x2560, 320x320, 100x100, inputPhoto (currently uploaded)

    @classmethod
    def convert(cls, data, client):
        file_data = None
        # find most bigger photo by preferred sizes list
        for size in cls.preferred_sizes:
            for s in data['sizes']:
                if s['type'] == size:
                    file_data = s['photo']
                    break
            if file_data:
                break
        if file_data:
            return client.get_or_download_file(file_data, photos_name_generator)
        return None


class  TelegramMessageConverter:
    known_entities = {
        'textEntityTypeBold': TEXT_ENTITY_BOLD,
        'textEntityTypeItalic': TEXT_ENTITY_ITALIC,
        'textEntityTypeTextUrl': TEXT_ENTITY_TEXT_URL,
        'textEntityTypeUrl': TEXT_ENTITY_URL,
        'textEntityTypeEmailAddress': TEXT_ENTITY_EMAIL,
        'textEntityTypePhoneNumber': TEXT_ENTITY_PHONE
    }

    @classmethod
    def convert_entities(cls, content_data):
        result = []
        for ent in content_data:
            etype = cls.known_entities.get(ent['type']['@type'])
            if etype:
                e = {
                    "t": etype,
                    "s": ent['offset'],
                    "l": ent['length']
                }
                if etype == TEXT_ENTITY_TEXT_URL:
                    e['v'] = ent['type']['url']

                result.append(e)

        if result:
            return result
        else:
            return None

    @classmethod
    def convert_content(cls, content_data, client, chat_id, message_id, sender_id=0, ):
        type = content_data['@type']

        if type == 'messageText':
            if content_data['text']['entities']:
                return {
                    'type': MESSAGE_TYPE_TEXT,
                    'text': content_data['text']['text'],
                    'entities': cls.convert_entities(content_data['text']['entities'])
                }
            else:
                return {
                    'type': MESSAGE_TYPE_TEXT,
                    'text': content_data['text']['text']
                }

        elif type == 'messagePhoto':
            text = content_data['caption']['text']
            photo = TelegramPhotoConverter.convert(content_data['photo'], client)
            if photo:
                try:
                    photo_file = os.path.join(settings.MEDIA_ROOT, photo)
                    image = Image.open(photo_file)
                    thumb = get_thumbnailer(photo).get_thumbnail(settings.ATTACHMENT_THUMBNAIL_CONF)
                    attachment = {
                        'url': build_absolute_uri(os.path.join(settings.MEDIA_URL, photo)),
                        'size': os.path.getsize(photo_file),
                        'width': image.width,
                        'height': image.height,
                        'thumb_url': build_absolute_uri(thumb.url),
                        'thumb_width': thumb.width,
                        'thumb_height': thumb.height
                    }
                    return {
                        'type': MESSAGE_TYPE_PHOTO,
                        'text': text,
                        'attachment_image': attachment
                    }
                except Exception as ex:
                    logger.warning(u"Image error: {}".format(ex))
                    return {
                        'type': MESSAGE_TYPE_TEXT,
                        'text': "(this image type not handled yet..)"
                    }
            else:
                return {
                    'type': MESSAGE_TYPE_TEXT,
                    'text': text
                }

        elif type == 'messageVideo':
            text = content_data['caption']['text']
            #print(content_data['video'])
            video_thumbnail = client.get_or_download_file(content_data['video']['thumbnail']['photo'], videos_name_generator)
            video_file = client.telegram_get_local_file(content_data['video']['video'], videos_name_generator)
            if video_thumbnail:
                try:
                    size = content_data['video']['video'].get('size', content_data['video']['video']['expected_size'])
                    if video_file:
                        video_url = build_absolute_uri(os.path.join(settings.MEDIA_URL, video_file))
                    else:
                        video_url = build_absolute_uri(reverse("api:telegram_download",
                                            kwargs={'file_kind': 'videos', 'file_id': content_data['video']['video']['id']}))
                    thumb = get_thumbnailer(video_thumbnail).get_thumbnail(settings.ATTACHMENT_THUMBNAIL_CONF)
                    attachment = {
                        'url': video_url,
                        'size': size,
                        'name': content_data['video']['file_name'],
                        'width': content_data['video']['width'],
                        'height': content_data['video']['height'],
                        'thumb_url': build_absolute_uri(thumb.url),
                        'thumb_width': thumb.width,
                        'thumb_height': thumb.height,
                        'duration': content_data['video']['duration'],
                        'file_type': 0xFB2
                    }
                    return {
                        'type': MESSAGE_TYPE_VIDEO,
                        'text': text,
                        'attachment_file': attachment
                    }
                except Exception as ex:
                    logger.warning(u"Video thumbnail error: {}".format(ex))
                    return {
                        'type': MESSAGE_TYPE_TEXT,
                        'text': "(this video type not handled yet..)"
                    }
            else:
                return {
                    'type': MESSAGE_TYPE_TEXT,
                    'text': text
                }

        elif type == 'messageSticker':
            photo = client.get_or_download_file(content_data['sticker']['sticker'], stickers_name_generator)
            if photo:
                try:
                    photo_file = os.path.join(settings.MEDIA_ROOT, photo)
                    image = Image.open(photo_file)
                    thumb = get_thumbnailer(photo).get_thumbnail(settings.ATTACHMENT_THUMBNAIL_CONF)
                    attachment = {
                        'url': build_absolute_uri(os.path.join(settings.MEDIA_URL, photo)),
                        'size': os.path.getsize(photo_file),
                        'width': image.width,
                        'height': image.height,
                        'thumb_url': build_absolute_uri(thumb.url),
                        'thumb_width': thumb.width,
                        'thumb_height': thumb.height
                    }
                    return {
                        'type': MESSAGE_TYPE_STICKER,
                        'text': '',
                        'attachment_image': attachment
                    }
                except Exception as ex:
                    logger.warning(u"Image (sticker) error: {}".format(ex))
                    return {
                        'type': MESSAGE_TYPE_TEXT,
                        'text': content_data['sticker']['emoji']
                    }
            else:
                return {
                    'type': MESSAGE_TYPE_TEXT,
                    'text': content_data['sticker']['emoji']
                }

        elif type == 'messageDocument':
            text = content_data['caption']['text']
            doc = client.get_or_download_file(content_data['document']['document'], attachments_name_generator)
            if doc:
                file_type = mime_to_riscos_type(content_data['document']['mime_type'])
                doc_name = content_data['document']['file_name']
                doc_file = os.path.join(settings.MEDIA_ROOT, doc)
                attachment = {
                    'url': build_absolute_uri(os.path.join(settings.MEDIA_URL, doc)),
                    'size': os.path.getsize(doc_file),
                    'name': doc_name,
                    'file_type': file_type,
                    'is_image': riscos_is_image_type(file_type)
                }
                return {
                    'type': MESSAGE_TYPE_FILE,
                    'text': text,
                    'attachment_file': attachment
                }
            else:
                return {
                    'type': MESSAGE_TYPE_TEXT,
                    'text': text
                }

        elif type == 'messageChatAddMembers':
            memids = content_data['member_user_ids']
            added_members = []
            for memid in memids:
                user_data = client.call_method("getUser", user_id=memid)
                added_members.append(user_data['first_name'])

            if len(memids) > 1:
                sender = client.call_method("getUser", user_id=sender_id)
                txt = _("{} added {}").format(sender['first_name'], ",".join(added_members))
            else:
                if len(added_members) > 0:
                    txt = _("{} joined").format(added_members[0])
                else:
                    txt = "someone joined (should never happens)"

            return {
                'type': MESSAGE_TYPE_JOIN,
                'text': txt
            }

        elif type == 'messageChatJoinByLink':
            sender = client.call_method("getUser", user_id=sender_id)
            txt = _("{} joined").format(sender['first_name'])
            return {
                'type': MESSAGE_TYPE_JOIN,
                'text': txt
            }

        elif type == 'messageContactRegistered':
            return {
                'type': MESSAGE_TYPE_JOIN,
                'text': _("Contact registered in Telegram")
            }

        elif type == 'messageChatChangeTitle':
            return {
                'type': MESSAGE_TYPE_TEXT,
                'text': _("Chat title changed to: ") + content_data['title']
            }

        else:
            return {
                'type': MESSAGE_TYPE_TEXT,
                'text': type + " (not handled yet..)"
            }

    @classmethod
    def convert_reply(cls, data, client, chat_id):
        result = {
            'id': data['id'],
            'author_id': MESSENGER_TG + str(data['sender_user_id']) if data['sender_user_id'] > 0 else "",

        }
        result.update(cls.convert_content(data['content'], client, chat_id, data['id'], sender_id=data.get('sender_user_id', 0)))
        return result

    @classmethod
    def convert(cls, data, client, with_reply_info=False):
        flags = 0
        if data['is_outgoing']:
            flags = flags | MESSAGE_FLAG_OUTGOING
        if data['can_be_edited']:
            flags = flags | MESSAGE_FLAG_CAN_BE_EDITED
        result = {
            'id': data['id'],
            'author_id': MESSENGER_TG + str(data['sender_user_id']) if data['sender_user_id'] > 0 else "",
            'chat_id': MESSENGER_TG + str(data['chat_id']),
            'flags': flags,
            'changedtime': data['edit_date'],
            'sendtime': data['date'],
        }
        sending_state = data.get('sending_state')
        if sending_state:
            if sending_state['@type'] == 'messageSendingStatePending':
                result['sending_state'] = MESSAGE_SENDING_STATE_PENDING
            elif sending_state['@type'] == 'messageSendingStateFailed':
                result['sending_state'] = MESSAGE_SENDING_STATE_FAILED

        result.update(cls.convert_content(data['content'], client, data['chat_id'], data['id'], data['sender_user_id']))

        if 'forward_info' in data and 'origin' in data['forward_info']:
            origin = data['forward_info']['origin']
            if 'sender_user_id' in origin:
                try:
                    from_user = client.call_method("getUser", user_id=origin['sender_user_id'])
                    result['forward_info'] = {'title': "{} {}".format(from_user['first_name'], from_user['last_name']) if from_user['last_name'] else from_user['first_name'],
                                'user_id': MESSENGER_TG + str(origin['sender_user_id'])
                                }
                except TelegramException as ex:
                    logger.error("getUser error: %s (no forward_info set) for message id:%s", ex, result['id'])

            elif 'chat_id' in origin:
                try:
                    from_channel = client.call_method("getChat", chat_id=origin['chat_id'])
                    result['forward_info'] = {'title': from_channel['title'],
                                    'chat_id': MESSENGER_TG + str(origin['chat_id'])
                                }
                except TelegramException as ex:
                    logger.error("getChat error: %s (no forward_info set) for message id:%s", ex, result['id'])

        if with_reply_info:
            reply_to_id = data.get('reply_to_message_id', 0)
            if reply_to_id != 0:
                try:
                    replied_message = client.call_method("getMessage", chat_id=data['chat_id'], message_id=reply_to_id)
                    if replied_message:
                        result['reply_info'] = cls.convert_reply(replied_message, client, data['chat_id'])
                except TelegramException as ex:
                    logger.error("getMessage error: %s (no reply_info set) for message %s", ex, result['id'])

        return result

class TelegramUserConverter:
    generate_pic_name = MD5PathWithExt("telegram/profile")
    default_pic = "default/photo_profile.png"

    @classmethod
    def get_pic(cls, data, client):
        if 'profile_photo' not in data:
            return cls.default_pic

        if 'big' in data['profile_photo']:
            return client.get_or_download_file(data['profile_photo']['big'], cls.generate_pic_name, cls.default_pic)
        else:
            return client.get_or_download_file(data['profile_photo']['small'], cls.generate_pic_name, cls.default_pic)

    @classmethod
    def convert_status(cls, status_data):
        status_type = status_data['@type']
        if status_type == 'userStatusOffline':
            was_online = status_data['was_online']
        elif status_type == 'userStatusOnline':
            was_online = int(time.time())
        else:
            was_online = 0

        return {
            'online': status_data['@type'] == 'userStatusOnline',
            'was_online': was_online
        }

    @classmethod
    def get_displayname(cls, data):
        return "{} {}".format(data['first_name'], data['last_name']) if data['last_name'] else data['first_name']

    @classmethod
    def convert(cls, data, client):
        try:
            pic = cls.get_pic(data, client)
            pic_small_url = build_absolute_uri(
                get_thumbnailer(pic).get_thumbnail(settings.SMALL_PROFILE_THUMBNAIL_CONF).url)
            pic_url = build_absolute_uri(settings.MEDIA_URL + pic)
        except Exception as ex:
            logger.warning(u"Image (profile) error: {}".format(ex))
            pic_small_url = build_absolute_uri(get_thumbnailer(_default_profile_photo).get_thumbnail(settings.SMALL_PROFILE_THUMBNAIL_CONF).url)
            pic_url = build_absolute_uri(settings.MEDIA_URL + _default_profile_photo)

        uid = MESSENGER_TG + str(data['id'])
        result = {
            'id': uid,
            'displayname': cls.get_displayname(data),
            'userid': data.get('username', ''),
            'pic': pic_url,
            'pic_small': pic_small_url,
            'first_name': data['first_name'],
            'last_name': data['last_name'],
            'phone': data.get('phone_number', ''),
            'email': '',
            'date_joined': 0
        }
        result.update(cls.convert_status(data['status']))
        return result

class TelegramChatConverter:
    chat_types = {
        'chatTypePrivate': CHAT_TYPE_PRIVATE,
        'chatTypeSecret': CHAT_TYPE_SECRET,
        'chatTypeBasicGroup' : CHAT_TYPE_GROUP,
        'chatTypeSupergroup': CHAT_TYPE_GROUP
    }
    generate_pic_name = MD5PathWithExt("telegram/chat")
    default_pic = "default/photo_profile.png"

    @classmethod
    def get_pic(cls, data, client):
        if 'photo' not in data:
            return cls.default_pic
        logger.debug("Get pic for chat {} {}".format(data['id'], data['title']))
        return client.get_or_download_file(data['photo']['small'], cls.generate_pic_name, cls.default_pic)

    @classmethod
    def convert(cls, data, client, full_info=True):
        if data['type']['@type'] == 'chatTypeSupergroup' and data['type']['is_channel']:
            chat_type = CHAT_TYPE_CHANNEL
        else:
            chat_type = cls.chat_types[data['type']['@type']]

        mystatus = CHATMEMBER_STATUS_NORMAL if data['permissions']['can_send_messages'] \
            else CHATMEMBER_STATUS_READONLY

        if full_info:
            pic = cls.get_pic(data, client)
            pic_small_url = build_absolute_uri(
                get_thumbnailer(pic).get_thumbnail(settings.SMALL_PROFILE_THUMBNAIL_CONF).url)
            pic_url = build_absolute_uri(settings.MEDIA_URL + pic)

            result = {
                'id': MESSENGER_TG + str(data['id']),
                'title': data['title'],
                'pic_small': pic_small_url,
                'pic': pic_url,
                'type': chat_type,
                'unread_count': data['unread_count'],
                'outgoing_seen_message_id': data['last_read_outbox_message_id'],
                'incoming_seen_message_id': data['last_read_inbox_message_id'],
                'my_status': mystatus,
                # 'can_send_messages': data['permissions']['can_send_messages'],
                # 'can_send_media_messages': data['permissions']['can_send_media_messages'],
            }
            if chat_type == CHAT_TYPE_PRIVATE or chat_type == CHAT_TYPE_SECRET:
                result['members_count'] = 1
                result['member'] = client.get_user_as_dict(data['type']['user_id'])
            elif data['type']['@type'] == 'chatTypeBasicGroup':
                group_data = client.get_basic_group(data['type']['basic_group_id'])
                result['members_count'] = group_data['member_count'] - 1
                result['member'] = None

            last_message = data.get('last_message')
            if last_message:
                result['last_msg'] = TelegramMessageConverter.convert(last_message, client)
            return result

        else:
            return {
                'id': MESSENGER_TG + str(data['id']),
                'title': data['title'],
                'type': chat_type,
                'unread_count': data['unread_count'],
                'outgoing_seen_message_id': data['last_read_outbox_message_id'],
                'incoming_seen_message_id': data['last_read_inbox_message_id'],
                'my_status': mystatus,
                'can_send_messages': data['permissions']['can_send_messages'],
                'can_send_media_messages': data['permissions']['can_send_media_messages'],
            }


class IKTelegramClient(IKAbstractChatClient, IKTelegramBase):
    @staticmethod
    def get_client(me, raise_exception=True):
        if not hasattr(me, "_tg_client"):
            try:
                tgacc = me.telegram_account
            except models.ObjectDoesNotExist:
                if not raise_exception:
                    return None
                msg = "Member {} has no telegram account".format(me)
                logger.error(msg)
                raise NotFound(msg, 'telegram:has_no_account')

            me._tg_client = IKTelegramClient(tgacc)

        return me._tg_client

    @staticmethod
    def register_account(member, phone):
        acc, _created = TelegramAccount.objects.get_or_create(member=member, phone=phone)
        client = IKTelegramClient.get_client(acc.member)
        client.telegram_send_request(client.me_id, "start", phone=client.account.phone)
        return client

    @staticmethod
    def change_account_phone(member, phone):
        acc = TelegramAccount.objects.filter(member=member).first()
        if acc:
            logger.error("Telegram account does not exists")
        client = IKTelegramClient(acc)
        client.stop()
        time.sleep(1)
        acc.phone = phone
        acc.save()
        client = acc.member._tg_client = IKTelegramClient(acc)
        client.telegram_send_request(client.me_id, "start", phone=client.account.phone)
        return client

    def __init__(self, telegram_account):
        IKAbstractChatClient.__init__(self, telegram_account.member)
        IKTelegramBase.__init__(self)

        self.me_id = telegram_account.member_id
        self.account = telegram_account
        self.tg_me = None

    def call_method(self, name, tg_timeout=45, tg_start_wait_timeout=10, **kwargs):
        result = self.telegram_call_method(self.me_id, name, tg_timeout=tg_timeout, **kwargs)
        if result['@type'] != 'error':
            return result

        # error case
        details_code = result.get('details_code', '')
        if details_code == 'not_run' and tg_start_wait_timeout > 0:
            self.start()
            time.sleep(1)
            logger.info("Telegram not started, starting. Method:{}".format(name))
            return self.call_method(name, tg_timeout=tg_timeout, tg_start_wait_timeout=tg_start_wait_timeout-1, **kwargs)
        if details_code == 'must_wait':
            if tg_timeout > 1:
                time.sleep(1)
                logger.info("Telegram not ready, waiting... Method:{} Timeout:{}".format(name, tg_timeout))
                return self.call_method(name, tg_timeout=tg_timeout-1, tg_start_wait_timeout=tg_start_wait_timeout, **kwargs)
            else:
                logger.error("Telegram still not ready. Raising exception. Method:%s", name)
                raise TelegramException('Telegram instance not ready', 0, 'not_ready')

        if name == 'downloadFile' and result['code'] == 400:
            logger.warning("Telegram error, method={} params={} result={}".format(name, kwargs, result))
        else:
            logger.error("Telegram error, method={} params={} result={}".format(name, kwargs, result))
        if result['code'] == 401:
            raise TelegramUnauthorizedException(result['message'], result['code'], details_code)
        else:
            raise TelegramException(result['message'], result['code'], details_code)

    def start(self):
        self.telegram_send_request(self.me_id, "start", phone=self.account.phone)

    def stop(self):
        self.telegram_send_request(self.me_id, "stop")

    def register_user(self, first_name, last_name):
        return self.call_method("registerUser", first_name=first_name, last_name=last_name)

    def accept_tos(self, tos_id):
        return self.call_method("acceptTermsOfService", terms_of_service_id=tos_id)

    def delete_account(self, reason=""):
        self.call_method("deleteAccount", reason=reason)
        self.stop()
        self.account.delete()

    def resend_code(self):
        logger.debug('Asking to Resend Telegram code')
        return self.call_method('resendAuthenticationCode')

    def check_code(self, code):
        logger.debug('Sending Telegram code: %s', code)
        return self.call_method('checkAuthenticationCode', code=str(code))

    def check_password(self, password):
        logger.debug('Sending Telegram password: %s', password)
        return self.call_method('checkAuthenticationPassword', password=str(password))

    def get_me(self):
        if not self.tg_me:
            user_data = self.call_method("getMe", tg_timeout=120, tg_start_wait_timeout=60)
            self.tg_me = TelegramUserConverter.convert(user_data, self)
            self.tg_me['user_id'] = user_data['id']

            if self.account.tg_user_id != user_data['id']:
                self.account.tg_user_id = user_data['id']
                self.account.save()
        return self.tg_me

    def set_my_profile_photo(self, photo_file):
        photo_file = self._save_temp_uploaded_file(photo_file)

        photo_content = {
            '@type': 'inputFileLocal',
            'path': photo_file
        }
        self.call_method("setProfilePhoto", photo=photo_content)
        return build_absolute_uri(settings.MEDIA_URL + photo_file[len(settings.MEDIA_ROOT)+1:])

    def set_my_avatar(self, avatar):
        if avatar and default_storage.exists(avatar):
            photo_content = {
                '@type': 'inputFileLocal',
                'path': default_storage.path(avatar)
            }
            self.call_method("setProfilePhoto", photo=photo_content)
            return build_absolute_uri(settings.MEDIA_URL + avatar)
        else:
            return ''

    def get_user_as_dict(self, tg_user_id, raise_not_found_exception=True):
        try:
            user_data = self.call_method("getUser", user_id=tg_user_id)
            return TelegramUserConverter.convert(user_data, self)
        except TelegramException as ex:
            if ex.code == 404 and not raise_not_found_exception:
                return None
            raise

    def get_users_as_dict(self, tg_user_ids, raise_not_found_exception=True):
        return [self.get_user_as_dict(uid, raise_not_found_exception=True) for uid in tg_user_ids]

    def get_chat(self, tg_chat_id, full_info=True):
        chat_data = self.call_method("getChat", chat_id=tg_chat_id)
        return TelegramChatConverter.convert(chat_data, self, full_info=full_info)

    def get_chat_as_dict(self, tg_chat_id):
        return self.get_chat(tg_chat_id)

    def get_chats_as_dict(self):
        result = self.call_method("getChats", offset_order=9223372036854775807, limit=100)
        chats = []
        for id in result['chat_ids']:
            chats.append(self.get_chat(id, full_info=True))

        return chats

    def get_chat_members(self, tg_chat_id, search, limit):
        chat_data = self.call_method("getChat", chat_id=tg_chat_id)
        result = []
        if chat_data['type']['@type'] == 'chatTypeBasicGroup':
            group_info = self.call_method("getBasicGroupFullInfo", basic_group_id=chat_data['type']['basic_group_id'])
            for m in group_info['members']:
                if m['user_id'] == self.account.tg_user_id:
                    continue
                if m['status']['@type'] == 'chatMemberStatusMember':
                    status = CHATMEMBER_STATUS_NORMAL
                elif m['status']['@type'] == 'chatMemberStatusCreator':
                    status = CHATMEMBER_STATUS_CREATOR
                elif m['status']['@type'] == 'chatMemberStatusAdministrator':
                    status = CHATMEMBER_STATUS_ADMIN
                elif m['status']['@type'] == 'chatMemberStatusBanned':
                    status = CHATMEMBER_STATUS_BANNED
                elif m['status']['@type'] == 'chatMemberStatusRestricted':
                    status = CHATMEMBER_STATUS_READONLY
                else:
                    continue

                mem = self.call_method("getUser", user_id=m['user_id'])
                memdata = TelegramUserConverter.convert(mem, self)
                if search and search not in memdata['displayname']:
                    continue

                result.append({
                    'chat_id': MESSENGER_TG + str(tg_chat_id),
                    'status': status,
                    'member': memdata
                })
                limit -= 1
                if limit <= 0:
                    break
        return result

    def get_basic_group_members(self, basic_group_id, limit=5):
        group_info = self.call_method(self.me_id, "getBasicGroupFullInfo", basic_group_id=basic_group_id)
        result = []
        for m in group_info['members']:
            if m['user_id'] == self.account.tg_user_id:
                continue
            if m['status']['@type'] == 'chatMemberStatusLeft':
                continue
            mem = self.get_user_as_dict(m['user_id'])
            result.append(mem)
            limit -= 1
            if limit <= 0:
                break
        return result

    def get_basic_group(self, basic_group_id):
        return self.telegram_call_method(self.me_id, "getBasicGroup", basic_group_id=basic_group_id)


    def _append_message(self, msg_dict, messages, replies):
        convmsg = TelegramMessageConverter.convert(msg_dict, self)
        if convmsg:
            if msg_dict['reply_to_message_id'] > 0:
                if msg_dict['reply_to_message_id'] not in replies:
                    replies[msg_dict['reply_to_message_id']] = [convmsg]
                else:
                    replies[msg_dict['reply_to_message_id']].append(convmsg)
            messages.append(convmsg)

    def get_chat_history(self, tg_chat_id, from_message_id=0, limit=20, filter=None, offset=0):
        limit = min(100, limit)

        if offset < 0 and limit > -offset:
            overlimit = limit + 2
            offset -= 1
        else:
            overlimit = limit + 1

        chat_data = self.call_method("getChatHistory", chat_id=tg_chat_id, from_message_id=from_message_id, limit=overlimit, offset=offset)
        if len(chat_data['messages']) == 0:
            return [], False, False


        messages = []
        replies = {}
        has_more_older = False
        has_more_newer = False
        count_newer = 0
        count_older = 0

        for msg in chat_data['messages']:
            logger.debug("message id = %s %s", msg['id'], from_message_id == msg['id'])
            if from_message_id > 0:
                if msg['id'] >= from_message_id:
                    count_newer += 1
                else:
                    count_older += 1
            else:
                count_older += 1
            self._append_message(msg, messages, replies)


        if count_older >= overlimit + offset:
            has_more_older = True
            del messages[-1]
        if offset < 0 and count_newer >= -offset - 1:
            has_more_newer = True
            del messages[0]

        logger.debug("count_newer %s count_older %s overlimit + offset %s offset %s has_more_newer %s has_more_older %s -offset - 1 %s", count_newer, count_older, overlimit + offset, offset, has_more_newer, has_more_older, -offset - 1)

        if len(messages) < limit:
            overlimit = overlimit - len(messages)
            if offset != 0:
                if offset < 0:
                    from_message_id = chat_data['messages'][0]['id']
                    offset = -overlimit
                else:
                    from_message_id = chat_data['messages'][-1]['id']
                    offset = 0
            else:
                from_message_id = chat_data['messages'][-1]['id']

            chat_data = self.call_method("getChatHistory", chat_id=tg_chat_id,
                                         from_message_id=from_message_id,
                                         limit=overlimit,
                                         offset=offset)

            if len(chat_data['messages']) == overlimit:
                if offset < 0:
                    has_more_newer = True
                    del chat_data['messages'][0]
                else:
                    has_more_older = True
                    del chat_data['messages'][-1]


            for msg in chat_data['messages']:
                logger.debug("2message id = %s %s", msg['id'], from_message_id == msg['id'])
                if msg['id'] != from_message_id:
                    self._append_message(msg, messages, replies)

        if messages:
            if replies:
                replied_messages = self.call_method("getMessages", chat_id=tg_chat_id, message_ids=list(replies.keys()))
                if replied_messages['messages']:
                    for rmsg in replied_messages['messages']:
                        if rmsg:
                            reply_info = TelegramMessageConverter.convert_reply(rmsg, self, tg_chat_id)
                            for conv_msg in replies[rmsg['id']]:
                                conv_msg['reply_info'] = reply_info

            return messages, has_more_older, has_more_newer
        else:
            return [], False, False

    def get_whole_chat_history_queryset(self, tg_chat_id):
        limit = 90*3

        chat_data = self.call_method("getChatHistory", chat_id=tg_chat_id, from_message_id=0, limit=90, offset=0)
        if len(chat_data['messages']) == 0:
            return []


        messages = []
        replies = {}

        for msg in chat_data['messages']:
            self._append_message(msg, messages, replies)

        while(len(messages) < limit):
            lim = min(90, limit - len(messages))
            from_message_id = chat_data['messages'][-1]['id']

            chat_data = self.call_method("getChatHistory", chat_id=tg_chat_id,
                                         from_message_id=from_message_id,
                                         limit=lim,
                                         offset=0)

            if len(chat_data['messages']) == 0:
                break

            for msg in chat_data['messages']:
                if msg['id'] != from_message_id:
                    self._append_message(msg, messages, replies)

        authors = {}
        for msg in messages:
            author_id = msg['author_id'][1:]
            if author_id not in authors:
                authors[author_id] = []
            authors[author_id].append(msg)

        for author in self.get_users_as_dict(authors.keys(), raise_not_found_exception=False):
            for msg in authors[author['id'][1:]]:
                msg['author'] = author

        return messages

    def get_message(self, tg_chat_id, message_id):
        msg_data = self.call_method("getMessage", chat_id=tg_chat_id, message_id=message_id)
        return TelegramMessageConverter.convert(msg_data, self)

    def get_or_download_file(self, file_data, name_generator, default_file='', priority=5):
        if file_data['local']['is_downloading_active']:
            return default_file

        local = self.telegram_get_local_file(file_data, name_generator)
        if local:
            return local

        try:
            new_file_data = self.call_method("downloadFile", file_id=file_data['id'], synchronous=True, priority=priority)
        except TelegramException as ex:
            logger.warning("get_or_download_file, download file_id: {} failed: {}".format(file_data['id'], ex))
            return default_file

        local = self.telegram_get_local_file(new_file_data, name_generator)
        if local:
            return local
        else:
            return default_file

    def get_or_download_file_async(self, file_data, name_generator, file_related_data):
        if file_data['local']['is_downloading_active']:
            return None

        local = self.telegram_get_local_file(file_data, name_generator)
        if local:
            return local

        try:
            download_not_starts = self.redis_conn.setnx("%s:%s:%s" % (self.TG_REDIS_FILE_DOWNLOAD_PREFIX, self.me_id, file_data['id']), file_related_data)
            if download_not_starts:
                new_file_data = self.call_method("downloadFile", file_id=file_data['id'], synchronous=False, priority=3)
                local = self.telegram_get_local_file(new_file_data, name_generator)
                if local:
                    return local
                else:
                    return None
        except TelegramException as ex:
            logger.warning("get_or_download_message_video_file_async, download file_id: {} failed: {}".format(file_data['id'], ex))
            return None

    def _save_temp_uploaded_file(self, uploaded_file):
        if not isinstance(uploaded_file, str):
            stored_name = default_storage.save(os.path.join("temp_uploads", str(self.me_id), str(time.time()), uploaded_file.name), uploaded_file)
            uploaded_file = default_storage.path(stored_name)
        return uploaded_file

    def check_send_permissions(self, chat: Dict[str, Any]) -> None:
        if not chat['can_send_messages']:
            logger.warning("Member [{}] can't send to Telegram chat [{}]".format(self.me, chat['id']))
            raise ValidationError("You can't send message in to this chat")

    def send_message_content(self, chat_id, input_content, reply_to_id=0):
        return self.call_method("sendMessage", chat_id=chat_id,
                         reply_to_message_id=reply_to_id,
                         input_message_content=input_content)

    def send_message_text(self, chat, text, reply_to_id=0):
        chat_id = chat['id'][1:] if isinstance(chat, dict) else chat

        input_content = {
            '@type': 'inputMessageText',
            'text': {
                '@type': 'formattedText',
                'text': text
            }
        }
        return self.send_message_content(chat_id, input_content, reply_to_id)

    def send_message_photo(self, chat, photo_file, caption=None, reply_to_id=0):
        chat_id = chat['id'][1:] if isinstance(chat, dict) else chat

        photo_file = self._save_temp_uploaded_file(photo_file)

        img = Image.open(photo_file)
        input_content = {
            '@type': 'inputMessagePhoto',
            'width': img.width,
            'height': img.height,
            'photo': {
                '@type': 'inputFileLocal',
                'path': photo_file
            }
        }
        if caption:
            input_content['caption'] = {'@type': 'formattedText', 'text': caption}
        return self.send_message_content(chat_id, input_content, reply_to_id)

    def send_message_file(self, chat, file, file_name=None, file_type=None, caption=None, reply_to_id=0):
        chat_id = chat['id'][1:] if isinstance(chat, dict) else chat

        file = self._save_temp_uploaded_file(file)

        input_content = {
            '@type': 'inputMessageDocument',
            'document': {
                '@type': 'inputFileLocal',
                'path': file
            }
        }
        if caption:
            input_content['caption'] = {'@type': 'formattedText', 'text': caption}
        return self.send_message_content(chat_id, input_content, reply_to_id)

    def send_message_sticker(self, chat, sticker, reply_to_id=None):
        chat_id = chat['id'][1:] if isinstance(chat, dict) else chat
        thumbnailer = get_thumbnailer(sticker)
        bigthumbnail = thumbnailer.get_thumbnail(settings.ATTACHMENT_THUMBNAIL_CONF)
        input_content = {
            '@type': 'inputMessageSticker',
            'sticker': {
                '@type': 'inputFileLocal',
                'path': bigthumbnail.path
            },
            'width': bigthumbnail.width,
            'height': bigthumbnail.height
        }
        return self.send_message_content(chat_id, input_content, reply_to_id)


    def get_or_create_private_chat(self, user_id, full_info=True):
        chat = self.call_method('createPrivateChat', user_id=user_id, force=False)
        return TelegramChatConverter.convert(chat, self, full_info=full_info)

    def create_group_chat(self, member_ids, title):
        chat = self.call_method('createNewBasicGroupChat', user_ids=member_ids, title=title)
        return TelegramChatConverter.convert(chat, self, full_info=True)

    def mark_seen(self, chat, message_id):
        chat_id = chat['id'][1:] if isinstance(chat, dict) else chat

        return self.call_method('viewMessages', chat_id=chat_id, message_ids=[message_id], force_read=True)

    def send_chat_action(self, tg_chat_id: str, action: int):
        type = CHAT2TELEGRAM_ACTIONS.get(action)
        if type is not None:
            return self.call_method('sendChatAction', chat_id=tg_chat_id, action={"@type": type})

    def forward_message(self, to_chat_ids, from_chat_id, message_id):
        for to_chat_id in to_chat_ids:
            self.call_method('forwardMessages', chat_id=to_chat_id,
                                from_chat_id=from_chat_id,
                                message_ids=[message_id],
                                as_album=False,
                                send_copy=False,
                                remove_caption=False
                                )

    def delete_messages(self, tg_chat_id, message_ids: list, unsend):
        self.call_method('deleteMessages', chat_id=tg_chat_id, message_ids=message_ids, revoke=unsend)
        return message_ids

    def delete_chat_history(self, tg_chat_id, remove_from_chat_list, unsend):
        chat = self.call_method('getChat', chat_id=tg_chat_id)

        if remove_from_chat_list:
            if chat['can_be_deleted_only_for_self'] == False:
                self.call_method('leaveChat', chat_id=tg_chat_id)
                return

        if chat['can_be_deleted_for_all_users'] == False:
            unsend = False

        self.call_method('deleteChatHistory', chat_id=tg_chat_id, remove_from_chat_list=remove_from_chat_list, revoke=unsend)

        if remove_from_chat_list:
            Event.send(self.account.member, Event.EV_CHAT_DELETED, {
                "chat_id": MESSENGER_TG + str(tg_chat_id)
            })
        else:
            Event.send(self.account.member, Event.EV_CHAT_CLEARED, {
                "chat_id": MESSENGER_TG + str(tg_chat_id)
            })

    def leave_chat(self, tg_chat_id):
        self.call_method('leaveChat', chat_id=tg_chat_id)

    def open_chat(self, tg_chat_id):
        self.call_method('openChat', chat_id=tg_chat_id)

    def get_contacts(self, except_in_chat=None):
        result = self.call_method('getContacts')
        contact_ids = set(result['user_ids'])
        if except_in_chat:
            chat_data = self.call_method('getChat', chat_id=except_in_chat)
            if chat_data['type']['@type'] == 'chatTypeBasicGroup':
                group_info = self.call_method("getBasicGroupFullInfo", basic_group_id=chat_data['type']['basic_group_id'])
                for m in group_info['members']:
                    if m['user_id'] in contact_ids:
                        contact_ids.remove(m['user_id'])

        return self.get_users_as_dict(contact_ids, False)

    def search_chats(self, query):
        result = self.call_method('searchPublicChats', query=query)
        chats = []
        for id in result['chat_ids']:
            chats.append(self.get_chat(id, full_info=True))
        return chats

    def set_chat_title(self, tg_chat_id, title):
        self.call_method("setChatTitle", chat_id=tg_chat_id, title=title)

    def set_chat_photo(self, tg_chat_id, photo_file):
        photo_file = self._save_temp_uploaded_file(photo_file)

        photo_content = {
            '@type': 'inputFileLocal',
            'path': photo_file
        }
        self.call_method("setChatPhoto", chat_id=tg_chat_id, photo=photo_content)

    def add_members_to_chat(self, chat_id, member_ids):
        for mid in member_ids:
            self.call_method("addChatMember", chat_id=chat_id, user_id=mid, forward_limit=20)

    def join_chat(self, tg_chat_id):
        self.call_method("joinChat", chat_id=tg_chat_id)

    def edit_message(self, tg_chat_id, message_id, text):
        input_content = {
            '@type': 'inputMessageText',
            'text': {
                '@type': 'formattedText',
                'text': text
            }
        }
        self.call_method("editMessageText", chat_id=tg_chat_id, message_id=message_id, input_message_content=input_content)

    def search_chat_messages(self, tg_chat_id, query, filter, from_message_id=0, limit=20, tg_sender_id=None):
        limit = min(100, limit)
        if tg_sender_id:
            sender = {
                '@type': 'messageSenderUser',
                'user_id': tg_sender_id
            }
        else:
            sender = None

        chat_data = self.call_method("searchChatMessages", chat_id=tg_chat_id,
                                     query=query,
                                     from_message_id=from_message_id,
                                     limit=1,
                                     offset=0,
                                     message_thread_id=0,
                                     sender=sender)

        if len(chat_data['messages']) == 0:
            return [], False, False

        found_msg_id = chat_data['messages'][0]['id']

        return self.get_chat_history(tg_chat_id, found_msg_id, limit=limit*2, offset=-limit)

    def download_video_file_from_message(self, tg_chat_id, message_id):
        msg = self.call_method("getMessage", chat_id=tg_chat_id, message_id=message_id)
        if msg:
            video_file = self.get_or_download_file_async(msg['content']['video']['video'], videos_name_generator, "msgvideo:%s:%s" % (tg_chat_id, message_id))
            if video_file:
                return build_absolute_uri(os.path.join(settings.MEDIA_URL, video_file))
            else:

                return ""
        return ""

    def download_file_by_id(self, file_id, name_generator, default_file):
        file_data = self.call_method("getFile", file_id=file_id)
        if file_data:
            return self.get_or_download_file(file_data, name_generator, default_file=default_file)
        else:
            return default_file

register_chat_client(MESSENGER_TG, IKTelegramClient)
