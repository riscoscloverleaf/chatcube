import os
import json
import logging
import datetime
import requests
from time import time
from MySQLdb import OperationalError
from django.conf import settings
from uwsgi_tasks import *

try:
    from django.utils.encoding import force_unicode as force_text
except ImportError:  # python3
    from django.utils.encoding import force_text
from django.db.models import Q
from django.db import connection, connections
from django.utils.timezone import now
from django_redis import get_redis_connection
from ik.models import Member, GlobalVars
from ik.events import Event
from ik.signals import chat_action
from ik import constants

logger = logging.getLogger("cc")

#_online_app_members = {}


def get_app_online_channels():
    online_app_channels = set()
    url = "http://127.0.0.1/chan-stats?id={}.m*".format(settings.SITE_CODE)
    response = requests.get(url, headers={'Host': settings.API_DOMAIN})
    response = json.loads(response.text)
    for ch in response['infos']:
        if ch['subscribers'] > 0:
            try:
                siteid, pushchannel = ch['channel'].split('.m')
                online_app_channels.add(pushchannel)
            except ValueError:
                pass
    return online_app_channels


class PeriodicTask(object):
    PERIOD = 86400
    _last_run = time()

    def get_period(self):
        return self.PERIOD

    def run(self, *args, **kwargs):
        pass

    def __call__(self, *args, **kwargs):
        try:
            dt_now = time()
            if dt_now - self.get_period() < self._last_run:
                return

            self.run()

            self._last_run = dt_now
        except OperationalError as ex:
            if ex.args[0] == 2006:  # mysql server gone away
                logger.warn(u"mysql server gone away in task: {}. Close connection".format(self.__class__.__name__))
                connection.close()
            else:
                logger.exception("OperationalError in task: {}".format(self.__class__.__name__))
        except Exception:
            logger.exception("Shit happens in task: {}".format(self.__class__.__name__))


class SetOnlineOfflineTask(PeriodicTask):
    PERIOD = 30
    DEAD_TIMEOUT = 120

    def run(self):
        logger.debug(u"Run dead members cleanup")
        online_channels = get_app_online_channels()
        # print("online_channels=", online_channels)
        #dt_now = time()
        online_members = Member.objects.select_related('online').filter(push_channel__in=online_channels)
        for omem in online_members:
            #_online_app_members[omem.id] = dt_now
            omem.online.set_online()

        # dead_time = dt_now - self.DEAD_TIMEOUT
        # for mid in list(_online_app_members.keys()):
        #     if _online_app_members[mid] < dead_time:
        #         # print("dead member=", mid)
        #         del _online_app_members[mid]

        # print("online_members=", _online_app_members.keys())
        dead_time = now() - datetime.timedelta(seconds=self.DEAD_TIMEOUT)
        dead_members = Member.objects.select_related('online').filter(online__was_online__lt=dead_time, online__online=True)

        for dead in dead_members:
            # logger.info("Member {} dead. Set offline".format(dead))
            dead.online.set_offline()

task_set_online_offline = SetOnlineOfflineTask()

# class StopChatActions(PeriodicTask):
#     PERIOD = 10
#     TYPING_DELAY = 20
#     def run(self):
#         redis_conn = get_redis_connection()
#         typing_chatmems = redis_conn.hgetall("ik:chat:action")
#         tm_now = int(time())
#         stopped_actions = []
#         for mem_id, action in typing_chatmems.items():
#             if action[0] < tm_now:
#                 stopped_actions.append((mem_id, action[1]))
#
#         if stopped_actions:
#             for (mem_id, chat_id) in stopped_actions:
#                 mem = Member.objects.get(pk=mem_id)
#                 chat_action.send(Event, member=mem, chat_id=chat_id, action=0)
#                 redis_conn.hdel("ik:chat:action", mem_id)

# task_chat_actions = StopChatActions()

@timer(seconds=10, executor=TaskExecutor.MULE)
def main_periodic_timer_job(signal_number):
    import uwsgi
    if uwsgi.is_locked():
        logger.warn(u"UWSGI still locked by other task")
        return

    uwsgi.lock()
    try:
        connections['default'].close()

        task_set_online_offline()
        # task_chat_actions()
    except Exception as ex:
        logger.exception("Shit happens in main_periodic_timer_job: %s", ex)
    uwsgi.unlock()
