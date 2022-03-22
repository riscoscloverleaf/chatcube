from django.conf import settings
from django.utils.encoding import force_text

def ik_cachekey(key):
    return 'ik_compressor.{}.{}'.format(settings.APP_VERSION, force_text(key))