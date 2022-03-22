# -*- coding: utf-8 -*-
from django.conf import settings


# for notified by email
ENABLE_EMAIL = getattr(settings, 'JSERRORLOGGING_ENABLE_MAIL', True)

MAIL_TO = getattr(settings, 'JSERRORLOGGING_MAIL_TO', settings.ADMINS)
MAIL_NOTIFY_INTERVAL = getattr(settings, 'JSERRORLOGGING_MAIL_NOTIFY_INTERVAL', 3600)

# for saved Django model
# ENABLE_MODEL = getattr(settings, 'JSERRORLOGGING_ENABLE_MODEL', True)
# LOG_MODEL = getattr(settings, 'JSERRORLOGGING_LOG_MODEL', 'jserrorlogging.Log')

# for logging
ENABLE_LOGGER = getattr(settings, 'JSERRORLOGGING_ENABLE_LOGGER', False)
LOGGER_NAME = getattr(settings, 'JSERRORLOGGING_LOGGER_NAME', None)
LOG_FORMAT = getattr(settings, 'JSERRORLOGGING_LOG_FORMAT', None)