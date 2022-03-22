#from django.db import connections
from django.conf import settings
from django.apps import AppConfig


class IKAppConfig(AppConfig):
    name = "ik"
    label = "ik"
    verbose_name = 'RiscOS chat'

    def ready(self):
        import logging
        import os

        under_uwsgi = False
        logging.config.dictConfig(settings.LOGGING)

        try:
            import uwsgi
            under_uwsgi = True
        except:
            pass

        from . import listeners  # noqa

        if under_uwsgi:
            if settings.TASKS_ENABLED:
                from .timer_tasks import set_uwsgi_callbacks
                set_uwsgi_callbacks()

            logging.getLogger("cc").info("Started {} inside uWSGI".format(settings.APP_VERSION))
        else:
            if settings.APP_DAEMON:
                logging.getLogger("cc").info("Started {} {} outside uWSGI".format(settings.APP_DAEMON, settings.APP_VERSION))
            else:
                logging.getLogger("cc").info("Started {} outside uWSGI".format(settings.APP_VERSION))
        #from . import cache_invalidating # noqa



