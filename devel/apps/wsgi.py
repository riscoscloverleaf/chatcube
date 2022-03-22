"""
WSGI config for mlbk project.

It exposes the WSGI callable as a module-level variable named ``application``.

For more information on this file, see
https://docs.djangoproject.com/en/1.8/howto/deployment/wsgi/
"""

import os
import sys

sys.path.append(os.path.abspath(os.path.join(os.path.abspath(os.path.split(__file__)[0]), '..', 'apps')))


while(True):
    try:
        from django.core.wsgi import get_wsgi_application
        application = get_wsgi_application()
    except Exception as ex:
        #if "MySQL" in ex:
         #   import time
         #   print("MySQL ERROR AT STARTUP. SLEEP 30 SEC")
         #   time.sleep(30)
         #   continue
        raise
    break

