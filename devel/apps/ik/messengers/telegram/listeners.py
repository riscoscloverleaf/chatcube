import os
import logging
from django.dispatch import receiver
from django.contrib.auth.signals import user_logged_in
from ik.signals import member_online_changed
from .client import IKTelegramClient
logger = logging.getLogger("cc")


@receiver(user_logged_in)
def on_user_logged_in(sender, request=None, user=None, **kwargs):
    client = IKTelegramClient.get_client(user, raise_exception=False)
    if client:
        client.start()
        logger.info(u"Member [%s] logged in. Starting Telegram", user)


@receiver(member_online_changed)
def on_member_online_changed(sender, online=None, **kwargs):
    client = IKTelegramClient.get_client(online.member, raise_exception=False)
    if client:
        if online.online:
            client.start()
            logger.info(u"Member [%s] online. Starting Telegram", online.member)
        else:
            client.stop()
            logger.info(u"Member [%s] offline. Stopping Telegram", online.member)

