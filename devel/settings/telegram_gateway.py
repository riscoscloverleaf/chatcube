import os
from settings import globalconf
globalconf.APP_TEST_MODE = True if os.getenv("APP_TEST_MODE")=="1" else False
globalconf.APP_DAEMON = "telegram_gateway"
from .commonconf import *
from .modules.telegram import *
