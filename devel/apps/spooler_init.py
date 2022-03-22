import os
import sys
from uwsgi_tasks import set_uwsgi_callbacks, django_setup

sys.path.append(os.path.abspath(os.path.join(os.path.abspath(os.path.split(__file__)[0]), '..', 'apps')))

set_uwsgi_callbacks()
django_setup()
