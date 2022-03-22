import time
import logging

from rest_framework.exceptions import ValidationError
from django.contrib.auth.signals import user_logged_in
from django.conf import settings
from django.dispatch import receiver
from django.utils.translation import ugettext as _
from django_redis import get_redis_connection
from ik.models import Chat, Member, OpenedChat
from ik.models.chat import get_common_online_private_members
from ik.utils.riscos import ChatCubeVersion
from ik.signals import *
from ik import constants
from .events import Event

logger = logging.getLogger("cc")

@receiver(user_logged_in)
def on_user_logged_in(sender, request=None, user=None, **kwargs):
    logger.debug(u"Member [%s] logged in", user)
    member_app_ver = request.POST.get("app_version",request.GET.get("app_version","1.0"))
    member_app_ver_int = ChatCubeVersion.get_version_int_from_str(member_app_ver)
    app_ver = ChatCubeVersion.get_version()
    app_ver_int = ChatCubeVersion.get_version_int_from_str(app_ver)

@receiver(member_signed_up)
def on_member_signed_up(sender, request=None, member=None, **kwargs):
    logger.info(u"Member [%s] signed up", member)

    # autocreate private chats with every member
    members = Member.objects.filter(is_active=True, is_staff=False).exclude(id=member.id)
    for mem in members:
        Chat.objects.get_or_create_private_chat(member, mem)

    for gid in settings.PUBLIC_CHATCUBE_GROUPS:
        chat = Chat.objects.get(pk=gid)
        chatmem = chat.join_chat(member)

@receiver(after_send_message)
def on_after_send_message(sender, message=None, **kwargs):
    # message.chat.last_message = message
    # message.chat.save(update_fields=['last_message'])

    chatmembers = message.chat.chatmembers.select_related("member").filter(member__online__online=True)
    for cmem in chatmembers:
        Event.send(cmem.member, Event.EV_MESSAGE_CREATED, message.as_dict(cmem.member, with_reply_info=True))

@receiver(message_changed)
def on_message_changed(sender, message=None, **kwargs):
    # logger.debug("on_message_changed {}".format(message))
    chatmembers = message.chat.chatmembers.select_related("member").filter(member__online__online=True)
    for cmem in chatmembers:
        Event.send(cmem.member, Event.EV_MESSAGE_UPDATED, message.as_dict(cmem.member))

@receiver(messages_deleted)
def on_messages_deleted(sender, me=None, chat=None, message_ids=[], unsend=False, **kwargs):
    if unsend:
        chatmembers = chat.chatmembers.select_related('member').filter(member__online__online=True)
        for cmem in chatmembers:
            Event.send(cmem.member, Event.EV_MESSAGES_DELETED, {
                "chat_id": constants.MESSENGER_CHATCUBE + str(chat.id),
                "message_ids": message_ids
            })
    else:
        Event.send(me, Event.EV_MESSAGES_DELETED, {
            "chat_id": constants.MESSENGER_CHATCUBE + str(chat.id),
            "message_ids": message_ids
        })

@receiver(chat_history_cleared)
def on_chat_history_cleared(sender, me=None, chat=None, unsend=False, **kwargs):
    if unsend and not chat.is_group:
        chatmembers = chat.chatmembers.select_related('member')
        for cmem in chatmembers:
            Event.send(cmem.member, Event.EV_CHAT_CLEARED, {
                "chat_id": constants.MESSENGER_CHATCUBE + str(chat.id)
            })
    else:
        Event.send(me, Event.EV_CHAT_CLEARED, {
            "chat_id": constants.MESSENGER_CHATCUBE + str(chat.id)
        })


@receiver(chatmember_created)
def on_chatmember_created(sender, chat=None, member=None, **kwargs):
    Event.send(member, Event.EV_CHAT_CREATED, chat.as_dict(member))

@receiver(chatmember_deleted)
def on_chatmember_deleted(sender, chat=None, member=None, **kwargs):
    Event.send(member, Event.EV_CHAT_DELETED, {"chat_id": constants.MESSENGER_CHATCUBE + str(chat.id)})

@receiver(chat_changed)
def on_chat_changed(sender, chat=None, **kwargs):
    chatmembers = chat.chatmembers.select_related('member').filter(member__online__online=True)
    for cmem in chatmembers:
        Event.send(cmem.member, Event.EV_CHAT_UPDATED, chat.as_dict(cmem.member))

@receiver(member_changed)
def on_member_changed(sender, member=None, **kwargs):
    logger.debug(u"Member [%s] changed", member)
    members = get_common_online_private_members(member)
    for mem in members:
        Event.send(mem, Event.EV_MEMBER_UPDATED, member.as_dict(mem))

@receiver(member_online_changed)
def on_member_online_changed(sender, online=None, **kwargs):
    logger.info(u"Member [%s] %s", online.member, "online" if online.online else "offline")

    # if member offline then delete opened chats
    if not online.online:
        OpenedChat.objects.filter(device__member=online.member).delete()

    members = get_common_online_private_members(online.member)
    for mem in members:
        Event.send(mem, Event.EV_MEMBER_UPDATED, online.as_dict(mem))

@receiver(member_active_changed)
def on_member_active_changed(sender, online=None, **kwargs):
    logger.info(u"Member [%s] %s", online.member, "active" if online.active else "inactive")

    members = get_common_online_private_members(online.member)
    for mem in members:
        Event.send(mem, Event.EV_MEMBER_UPDATED, online.as_dict(mem))

@receiver(chat_action)
def on_chat_action(sender, member=None, chat_id=None, action=None, **kwargs):
    chat_id = constants.MESSENGER_CHATCUBE + str(chat_id)
    mid = constants.MESSENGER_CHATCUBE + str(member.id)
    members = Member.objects.filter(devices__opened_chat__chat_id=chat_id).exclude(pk=member.id)
    mem_count = len(members)
    if mem_count < 10 and mem_count > 0:
        logger.debug("Action [%s] in chat_id [%s]", action, chat_id)
        for mem in members:
            Event.send(mem, Event.EV_CHAT_ACTION, {'chat_id': chat_id, 'member_id': mid, 'action': action})
