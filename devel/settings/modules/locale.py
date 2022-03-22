# -*- coding: utf-8 -*-
import os
from settings.globalconf import PROJECT_DIR

# Internationalization
# https://docs.djangoproject.com/en/1.8/topics/i18n/

LANGUAGES = [
    ('en', u'English'),
    ('de', u'Deutsch'),
    ('pl', u'Polski'),
    ('ru', u'Русский'),
    ('es', u'Español'),
    ('pt', u'Português'),
    ('it', u'Italiano'),
    ('fr', u'Français'),
    ('th', u'ภาษาไทย'),
    # ('ko', u'한국의'),
    ('ja', u'日本の'),
    ('zh-cn', u'中国语'),
]

LANGUAGE_CODE = 'en'

USE_I18N = True
USE_L10N = False

DATE_INPUT_FORMATS = (
    '%d.%m.%Y',  # '25.10.2006'
    '%d.%m.%y',  # '25.10.06'
)

DATETIME_INPUT_FORMATS = [
    '%Y-%m-%dT%H:%M:%S.%fZ',     # '2006-10-25 14:30:59'
    '%Y-%m-%dT%H:%M:%SZ',     # '2006-10-25 14:30:59'
    '%Y-%m-%d %H:%M:%S',     # '2006-10-25 14:30:59'
    '%Y-%m-%d %H:%M:%S.%f',  # '2006-10-25 14:30:59.000200'
    '%Y-%m-%d %H:%M',        # '2006-10-25 14:30'
    '%Y-%m-%d',              # '2006-10-25'
]

DATE_FORMAT = "d.m.Y"
DATETIME_FORMAT = 'd.m.Y H:i:s'
DATETIME_TAG_FORMAT = '%d.%m.%Y %H:%M:%S' # strftime syntax
TIME_FORMAT = "H:i:s"

LOCALE_PATHS = (os.path.join(PROJECT_DIR, 'locale'),)

USE_TZ = True
TIME_ZONE = 'Europe/Berlin'

#STATICI18N_PACKAGES = ('django.conf', 'ik')
# STATICI18N_ROOT = os.path.join(PROJECT_DIR, "apps", "ik", "static")

