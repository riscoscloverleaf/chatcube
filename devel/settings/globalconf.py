import os
# version is x.y.z
# increment any of number mean new release will created and go into production
# increment of Y will ask mobile member to upgrade app
# increment of X is big major change also will ask mobile member to upgrade app
APP_VERSION = "0.0.135"
# these 2 variables will overriden in conf_XX and local_test.py, local_prod.py
APP_TEST_MODE = False
# daemon mode
APP_DAEMON = ""
# project directory
PROJECT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))