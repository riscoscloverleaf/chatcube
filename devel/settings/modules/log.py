import os
from settings.globalconf import APP_DAEMON, APP_TEST_MODE, PROJECT_DIR

LOGGING_CONFIG = None

LOGGING = {
    'version': 1,
    'formatters': {
        'simple': {
            'format': u'%(asctime)s: %(levelname)-8s %(funcName)s: %(message)s (%(ip_address)s) %(user)s) "%(url)s"',
            '()': 'ik.iklogging.IPUserFormatter'
        },
        "daemon": {
            'format': u'%(asctime)s %(levelname)-8s %(module)s:%(funcName)s: %(message)s'
        }
    },
    'handlers': {
        'console':{
            'level':'DEBUG',
            'class':'logging.StreamHandler',
            'formatter': 'simple',
        },
        'mail_admins': {
            'level': 'ERROR',
            'class': 'ik.iklogging.IKAdminEmailHandler',
            'include_html': True
        },
        'logfile': {
            'level': 'DEBUG',
            'class': 'logging.handlers.TimedRotatingFileHandler',
            'formatter': 'daemon',
            'filename': os.path.join(PROJECT_DIR, '..', 'log',
                                     ("dev_" if "/devel/" in os.path.abspath(__file__) else "") + APP_DAEMON + ('_test.log' if APP_TEST_MODE else '_prod.log')),
            'when': 'W0',
            'interval': 4,
            'backupCount': 10
        },
    },
    'loggers': {
        # 'django.profiler': {
        #     'handlers': ['logfile'],
        #     'level': 'DEBUG',
        #     'propagate': True,
        # },
        'cc': {
            # 'handlers': log_handlers,
            'level': 'DEBUG' if APP_TEST_MODE else 'INFO',
            # 'propagate': False,
        },
        'uwsgi_tasks': {
        #     'handlers': log_handlers,
            'level': 'ERROR',
        #     'propagate': False,
        },
        'django.db': {
        #     'handlers': log_handlers,
            'level': 'DEBUG',
        #     # 'propagate': False,
        },
        'django': {
        #     'handlers': log_handlers,
        #     #'level': 'DEBUG',
            'level': 'INFO',
        #     # 'propagate': False,
        },
        'easy_cache': {
        #     'handlers': log_handlers,
            'level': 'INFO',
        #     'propagate': False,
        },
        'requests': {
        #     'handlers': log_handlers,
            'level': 'ERROR',
        #     'propagate': False,
        },
        'PIL': {
        #     'handlers': log_handlers,
            'level': 'INFO',
        #     'propagate': False,
        },
        '': {
            'handlers': ['mail_admins', 'logfile'] if APP_DAEMON else ['mail_admins', 'console'],
            'level': 'DEBUG'
        },
    }
}
