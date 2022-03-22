"""
Django settings
"""
from settings.globalconf import *
from settings.modules.templates import *
from settings.modules.cache import *
from settings.modules.media import *
from settings.modules.log import *
from settings.modules.locale import *
from settings.modules.auth import *
from settings.modules.rest import *
#from settings.modules.xadmin import *
from settings.modules.application import *

DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.mysql',  # Add 'postgresql_psycopg2', 'mysql', 'sqlite3' or 'oracle'.
        'NAME': 'dbname',  # Or path to database file if using sqlite3.
        'USER': 'dbuser',  # Not used with sqlite3.
        'PASSWORD': 'dbpassw',  # Not used with sqlite3.
        'HOST': '',  # Set to empty string for localhost. Not used with sqlite3.
        'PORT': '',  # Set to empty string for default. Not used with sqlite3.
        'OPTIONS': { 'init_command': "SET sql_mode=STRICT_TRANS_TABLES" }

    },
    'old': {
        'ENGINE': 'django.db.backends.mysql',  # Add 'postgresql_psycopg2', 'mysql', 'sqlite3' or 'oracle'.
        'NAME': 'olddb',  # Or path to database file if using sqlite3.
        'USER': 'olddbu',  # Not used with sqlite3.
        'PASSWORD': 'xxxxxxxx',  # Not used with sqlite3.
        'HOST': '',  # Set to empty string for localhost. Not used with sqlite3.
        'PORT': '',  # Set to empty string for default. Not used with sqlite3.
        #'TIME_ZONE': 'Europe/Berlin',
        #        'OPTIONS': { 'charset': 'utf8' }
        'OPTIONS': { 'init_command': "SET sql_mode=STRICT_TRANS_TABLES" }

    },
}

SITE_ID = 1

# default config parameters
EMAIL_DOMAIN = "riscoscloverleaf.com"
DEFAULT_FROM_EMAIL = "info@"+EMAIL_DOMAIN
SERVER_EMAIL = "support@"+EMAIL_DOMAIN
EMAIL_SUBJECT_PREFIX = ''

SITE_NAME = "ChatCube"
SITE_CODE = "ROCHAT"
DEFAULT_HTTP_PROTOCOL = "https"

ALLOWED_HOSTS = ["localhost", "chatcube.org", "api.chatcube.org", "api2.chatcube.org", "api3.chatcube.org", "test.chatcube.org", "test2.chatcube.org"]

SECRET_KEY = 'yousecretkeyxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
FIELD_ENCRYPTION_KEYS = ('fieldencryptionkeyxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx',)

if APP_TEST_MODE:
    DEBUG = True
    TASKS_ENABLED = False
    API_DOMAIN = "test.chatcube.org"
else:
    DEBUG = False
    TASKS_ENABLED = True
    API_DOMAIN = "api.chatcube.org"

THUMBNAIL_DEBUG = DEBUG
TEMPLATES[0]['OPTIONS']['debug'] = DEBUG
#TEMPLATES[0]['OPTIONS']["auto_reload"] = DEBUG

CRISPY_FAIL_SILENTLY = not DEBUG
COMPRESS_ENABLED = not DEBUG

# group id where every user will added at regsitration
# RiscOS Coding 4219
# RiscOS Support 1
PUBLIC_CHATCUBE_GROUPS = [1, 4219]

XADMIN_TITLE = 'RiscOS chat admin'
XADMIN_FOOTER_TITLE = 'chatcube.org'
