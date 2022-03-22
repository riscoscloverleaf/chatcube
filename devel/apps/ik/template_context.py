from django.conf import settings
from django.utils.translation import trans_real
from django.utils import timezone
from ik import constants

def ik_context(request):
    lang_code = trans_real.get_language()
    langs = trans_real.get_languages()
    ctx = {
        'DEBUG': settings.DEBUG,
        'STATIC_URL': settings.STATIC_URL,
        'LANGUAGES': settings.LANGUAGES,
        'LANGUAGE': langs[lang_code],
        'LANGUAGE_CODE': lang_code,
        # 'LANGUAGE_BIDI': translation.get_language_bidi(),
        'TIME_ZONE': timezone.get_current_timezone_name(),
        'CACHE_VERSION': settings.CACHE_VERSION_TAG,
        'request': request,
        'CONSTANTS': constants,
        'settings': settings,
    }
    return ctx
