import os
import logging
import polib
import json
from django.conf import settings
from django.utils.translation import get_language, to_locale, gettext
from django.views.i18n import get_formats
from django.core.files.storage import default_storage
from easy_cache import invalidate_cache_prefix
from easy_thumbnails.files import get_thumbnailer
from rest_framework.response import Response
from rest_framework.views import APIView
from rest_framework.permissions import AllowAny

from ik.utils.site import build_absolute_uri
from ik.utils.riscos import ChatCubeVersion
from ik.models import Member, Country
from ..permissions import IsAuthenticated

logger = logging.getLogger("cc")

def get_avatars(request):
    dirs, files = default_storage.listdir("avatars")
    avatars = []
    for f in files:
        file = "avatars/" + f
        thumbnail = get_thumbnailer(file).get_thumbnail(settings.SMALL_PROFILE_THUMBNAIL_CONF)
        avatars.append([build_absolute_uri(thumbnail.url, request), file])
    return avatars

def get_stickers(request):
    dirs, files = default_storage.listdir("stickers")
    result = []
    dirs = sorted(dirs)
    for d in dirs:
        dirs, files = default_storage.listdir("stickers/" + d)
        if len(files) > 0:
            files = sorted(files)
            group_image = "stickers/{}.png".format(d)
            group_image_thumbnail = get_thumbnailer(group_image).get_thumbnail(settings.SMALL_PROFILE_THUMBNAIL_CONF)
            group = {"name": d, "items": [], "pic_small": build_absolute_uri(group_image_thumbnail.url, request)}
            for f in files:
                file = "stickers/{}/{}".format(d, f)
                thumbnailer = get_thumbnailer(file)
                thumbnail = thumbnailer.get_thumbnail(settings.SMALL_PROFILE_THUMBNAIL_CONF)
                bigthumbnail = thumbnailer.get_thumbnail(settings.ATTACHMENT_THUMBNAIL_CONF)

                group["items"].append({"pic":  build_absolute_uri(bigthumbnail.url, request),
                                       "pic_small": build_absolute_uri(thumbnail.url, request),
                                       "name": file})

            result.append(group)
    return result

class UtilsResetCache(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        cache_prefix = request.GET.get('prefix')
        if cache_prefix:
            invalidate_cache_prefix(cache_prefix)
            return Response("ok")
        return Response("invalid")


class AvatarsView(APIView):
    permission_classes = (AllowAny,)

    def get(self, request, *args, **kwargs):
        return Response(get_avatars(request))


class StickersView(APIView):
    permission_classes = (AllowAny,)

    def get(self, request, *args, **kwargs):
        return Response(get_stickers(request))


def get_locale_catalog_data(locale):
    localdir = "locale"
    localedomain = "mobile"
    path = os.path.join(settings.PROJECT_DIR, localdir, locale, "LC_MESSAGES", localedomain + '.po')

    pofile = polib.pofile(path)
    metadata = pofile.ordered_metadata()
    plural = None
    for name, value in metadata:
        if name == 'Plural-Forms':
            plural = value
            if plural:
                # this should actually be a compiled function of a typical plural-form:
                # Plural-Forms: nplurals=3; plural=n%10==1 && n%100!=11 ? 0 :
                #               n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2;
                plural = \
                    [el.strip() for el in plural.split(';') if el.strip().startswith('plural=')][0].split('=', 1)[1]
            break

    catalog = {}
    for entry in pofile.translated_entries():
        if entry.msgstr:
            catalog[entry.msgid] = entry.msgstr
        elif entry.msgstr_plural:
            catalog[entry.msgid] = entry.msgstr_plural

    return catalog, plural

def get_choices_data():
    lang = get_language()
    if not lang:
        lang = 'en'
    locale = to_locale(lang)

    countries = Country.objects.all().order_by(locale)
    # country_names = [(c.pk, c.name) for c in countries]
    country_phones = [{'code': c.code, 'phone': c.phone_code, 'name': c.name} for c in countries if len(c.phone_code) > 0]

    return {
        # 'COUNTRY_CHOICES': country_names,
        'COUNTRY_CHOICES': country_phones,
    }


def get_locale_data():
    lang = get_language()
    if not lang:
        lang = 'en'
    locale = to_locale(lang)

    catalog, plural = get_locale_catalog_data(locale)
    choicesData = get_choices_data()

    localedata = {
        'catalog': catalog,
        'formats': get_formats(),
        'plural': plural,
        "choices": choicesData,
        'languages': settings.LANGUAGES
    }

    return localedata


class LoadLocaleCatalogView(APIView):
    permission_classes = (AllowAny,)
    def get(self, request, *args, **kwargs):
        lang = get_language()
        if not lang:
            lang = 'en'
        locale = to_locale(lang)
        catalog, plural = get_locale_catalog_data(locale)
        return Response({'catalog': catalog, 'plural': plural, 'languages': settings.LANGUAGES, 'formats': get_formats()})

class LogLine(APIView):
    permission_classes = (AllowAny,)
    def post(self, request, *args, **kwargs):
        logger.info(u"APP: {}".format(request.POST.get('message')))
        return Response("ok")


class LoadLocaleView(APIView):
    permission_classes = (AllowAny,)
    def get(self, request, *args, **kwargs):
        return Response(get_locale_data())


class LoadInitialView(APIView):
    permission_classes = (AllowAny,)
    def get(self, request, *args, **kwargs):
        #is_browser = request.GET.get('platform') == 'browser'

        data = {
            'version': settings.APP_VERSION,
            'app_version': ChatCubeVersion.get_version(),
            'avatars': get_avatars(request),
            'stickers': get_stickers(request),
            'locale': get_locale_data(),
            'choices': get_choices_data()
        }

        # app_ver_list = map(int, request.GET.get('version','0.0.0').split('.'))
        # app_ver = app_ver_list[0] * 10000 + app_ver_list[1] * 100 + app_ver_list[2]
        # if app_ver < 10001:
        #     data['message'] = gettext("Please update application to latest version.")
        return Response(data)


class ChangeMemberSettingView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        name = kwargs.get("name")
        val = json.loads(request.data["value"])
        request.user.settings.set_value(name, val)
        return Response("ok")

    def get(self, request, *args, **kwargs):
        name = kwargs.get("name")
        return Response(request.user.settings.get_value(name))

    def delete(self, request, *args, **kwargs):
        name = kwargs.get("name")
        request.user.settings.delete_value(name)
        return Response("ok")

# class TestView(APIView):
#     permission_classes = (AllowAny,)
#     def get(self, request, *args, **kwargs):
#         activate('es')
#         a = gettext("Profile with same email {0} exists already on {1}.").format("em", "st")
#         logger.info("test: {}".format(a))
#         return Response({"d": a})


class PingView(APIView):
    permission_classes = (AllowAny,)

    def get(self, request, *args, **kwargs):
        # access models to check that all is ok and db is working
        _t = Member.objects.all()[0]
        return Response("ok")
