import datetime
from django.conf import settings
from django.utils.timezone import now, utc
from django.utils.dateformat import format as date_format
from django.utils.translation import ungettext, get_language, to_locale
from ik.utils.globals import current_request
from transliterate import translit

_lang_codes = set()

def get_ik_language():
    lng = get_language()

    if not lng:
        return 'en'
    if len(_lang_codes) == 0:
        _lang_codes.update([l[0] for l in settings.LANGUAGES])

    lng = to_locale(lng)

    if lng.lower() not in _lang_codes:
        return 'en'
    if lng == 'zh_CN':
        return 'zh'
    return lng

def age_as_text(age):
    return ungettext(
        '%(age)d year',
        '%(age)d years',
        age) % {'age': age}

def date_time_since(dt):
    if type(dt) is datetime.date:
        dt = datetime.datetime(dt.year, dt.month, dt.day, 0,0,0, tzinfo=utc)
    d = now() - dt
    if int(d.days / 365) > 0:
        if (dt.year > 1900):
            return dt.strftime("%d.%m.%Y")
        else:
            return "-"
    if d.days > 0:
        return ungettext("{0} day ago", "{0} days ago", d.days).format(d.days)
    hours = int(d.seconds / 3600)
    if hours > 0:
        return ungettext("{0} hour ago", "{0} hours ago", hours).format(hours)
    minutes = int(d.seconds / 60)
    if minutes > 0:
        return ungettext("{0} minute ago", "{0} minutes ago", minutes).format(minutes)
    return ungettext("{0} minute ago", "{0} minutes ago", 1).format(1)

def url_strip_language(url):
    for l, lname in settings.LANGUAGES + [('None', '')]:
        l = '/'+l+'/'
        if url[:len(l)] == l:
            return url[len(l)-1:]
    return url


def transliterate(txt):
    lng = get_language()
    txt = txt.strip()
    if lng != 'ru':
        return translit(txt, 'ru', reversed=True)
    else:
        return txt
