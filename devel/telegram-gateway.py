#!../pythonenv/bin/python
import os
import sys
import django

def main():
    from django.conf import settings
    with open(os.path.join(settings.PROJECT_DIR, "..", "data", "run", "telegram-gateway.pid"), "w") as f:
        print(os.getpid(), file=f)
    from ik.messengers.telegram.tg_gateway import TelegramGatewayProcess
    TelegramGatewayProcess().loop()

if __name__ == "__main__":
    if sys.argv[1] == 'test':
        os.environ["APP_TEST_MODE"] = "1"
    else:
        os.environ["APP_TEST_MODE"] = "0"
    os.environ["DJANGO_SETTINGS_MODULE"] = "settings.telegram_gateway"

    sys.path.append(os.path.abspath(os.path.join(os.path.abspath(os.path.split(__file__)[0]), 'apps')))
    django.setup()

    main()