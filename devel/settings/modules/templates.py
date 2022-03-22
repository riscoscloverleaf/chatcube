import os
from settings.globalconf import PROJECT_DIR

TEMPLATED_EMAIL_TEMPLATE_DIR = "ik/emails/"
TEMPLATED_EMAIL_AUTO_PLAIN = False

TEMPLATES = [
    {
        "BACKEND": "django.template.backends.django.DjangoTemplates",
        "DIRS": [os.path.join(PROJECT_DIR, "templates")],
#        "APP_DIRS": False,
        "OPTIONS": {
            'loaders': [
                #'admin_tools.template_loaders.Loader',
                'django.template.loaders.filesystem.Loader',
                'django.template.loaders.app_directories.Loader',
            ],
            'context_processors': [
                'django.contrib.auth.context_processors.auth',
                'django.template.context_processors.debug',
                'django.contrib.messages.context_processors.messages',
                "ik.template_context.ik_context", # ik_context include static, i18n, tz, request context avariables
                                                            # this reduce number of function calls
            ]
        }
    },
]

CRISPY_TEMPLATE_PACK = 'bootstrap3'
