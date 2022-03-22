import os
from time import time
from settings.globalconf import PROJECT_DIR

EMAIL_BACKEND = 'django.core.mail.backends.smtp.EmailBackend'

#X_FRAME_OPTIONS = 'ALLOW-FROM *'

ADMINS = (
    ('Sergey Lentsov', 'serge.lenz@gmail.com'),
)

MANAGERS = ADMINS

# Application definition

INSTALLED_APPS = (
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    'django.contrib.sites',
    'django.contrib.sitemaps',
    #'suit',
    'django.contrib.admin',
    'crispy_forms',
    'templated_email',
    'rest_framework',
    'easy_thumbnails',
    'mobileapp',
    'ik',
    'ik.api',
    'ik.messengers.chatcube',
    'ik.messengers.telegram',
)

MIDDLEWARE = (
    'django.contrib.sessions.middleware.SessionMiddleware',
    'django.middleware.locale.LocaleMiddleware',
    'django.middleware.common.CommonMiddleware',
    'django.middleware.csrf.CsrfViewMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    #'django.contrib.auth.middleware.PersistentRemoteUserMiddleware',

    'ik.api.middleware.IKAPIMiddleware',

    'django.contrib.messages.middleware.MessageMiddleware',
    #'django.middleware.clickjacking.XFrameOptionsMiddleware',
    'django.middleware.security.SecurityMiddleware',
    'django_globals.middleware.Global',
)

# DATABASE_ROUTERS = ('dating.chatdb.dbrouter.DatingDbRouter',)

ROOT_URLCONF = 'urls'

WSGI_APPLICATION = 'wsgi.application'

SESSION_ENGINE = 'django.contrib.sessions.backends.file'
SESSION_FILE_PATH = os.path.join(PROJECT_DIR, "var", "global", "sessions")

# LOGIN_URL = '/login/'
# LOGOUT_URL = '/logout/'
LOGIN_REDIRECT_URL = '/'

IP2LOCATION_DB = os.path.join(PROJECT_DIR, "var", "global", "IP-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE.BIN")
IPV62LOCATION_DB = os.path.join(PROJECT_DIR, "var", "global", "IPV6-COUNTRY-REGION-CITY-LATITUDE-LONGITUDE.BIN")

CACHE_VERSION_TAG = str(time())

ADMIN_ROOT_URL = '/admin/'
