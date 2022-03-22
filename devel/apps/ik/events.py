import logging
import json
import requests
from django.conf import settings
from ik.api.renderer import json_dumps

from ik.models import Member

logger = logging.getLogger("cc")

class Event(object):
    EV_MESSAGE_CREATED = 'MESSAGE_CREATED'
    EV_MESSAGE_UPDATED = 'MESSAGE_UPDATED'
    EV_MESSAGES_DELETED = 'MESSAGES_DELETED'
    EV_CHAT_CREATED = 'CHAT_CREATED'
    EV_CHAT_UPDATED = 'CHAT_UPDATED'
    EV_CHAT_UPDATED_OUTBOX = 'CHAT_UPDATED_OUTBOX'
    EV_CHAT_CLEARED = 'CHAT_CLEARED'
    EV_CHAT_DELETED = 'CHAT_DELETED'
    EV_CHAT_ACTION = 'CHAT_ACTION'
    EV_MEMBER_CREATED = 'MEMBER_CREATED'
    EV_MEMBER_UPDATED = 'MEMBER_UPDATED'
    EV_TELEGRAM_AUTH_CODE = 'TELEGRAM_AUTH_CODE'
    EV_TELEGRAM_AUTH_PASSWORD = 'TELEGRAM_AUTH_PASSWORD'
    EV_TELEGRAM_AUTH_REGISTRATION = 'TELEGRAM_AUTH_REGISTRATION'
    EV_TELEGRAM_READY = 'TELEGRAM_READY'
    EV_TELEGRAM_TERMS = 'TELEGRAM_TERMS'
    EV_SHOW_ALERT = 'SHOW_ALERT'

    @classmethod
    def send(cls, to, evtype, object=None):
        if not isinstance(to, Member):
            to = Member.objects.get(to)

        url = "http://127.0.0.1/pub?id=m{}".format(to.push_channel)

        if not isinstance(object, dict):
            raise ValueError("Wrong object (non-dict): {}".format(object))

        ev_data = {
            'type': evtype,
            'data': object
        }
        post_data = json_dumps(ev_data)
        logger.debug("Event to {} [{}] event={}".format(to, to.push_channel, post_data))
        r = requests.post(url, post_data, headers={'host': settings.API_DOMAIN})
        if r.status_code != 200:
            logger.warn(u"Push event failed. Member: {} Data:{} Status:{} Result:{}".format(to.id, serialized_data, r.status_code, r.text))
