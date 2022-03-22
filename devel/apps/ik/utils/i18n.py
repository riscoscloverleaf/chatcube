from django.core.exceptions import ObjectDoesNotExist
from django.conf import settings
from django.utils.translation import get_language, activate, to_locale

_lang_codes = set()

# class MemberLanguageContext(object):
#     def __init__(self, mem):
#         self.member = mem
#         self.old_lng = get_language()
#
#     def __enter__(self):
#         try:
#             self.email_language = self.member.email_settings.email_language
#         except ObjectDoesNotExist:
#             self.email_language = 'en'
#
#         if self.old_lng != self.email_language:
#             activate(self.email_language)
#         return self.email_language
#
#     def __exit__(self, exc_type, exc_val, exc_tb):
#         if self.old_lng != self.email_language:
#             activate(self.old_lng)


# def get_ik_language():
#     lng = get_language()
#
#     if not lng:
#         return 'en'
#     if len(_lang_codes) == 0:
#         _lang_codes.update([l[0] for l in settings.LANGUAGES])
#
#     lng = to_locale(lng)
#
#     if lng.lower() not in _lang_codes:
#         return 'en'
#     if lng == 'ja':
#         return 'jp'
#     if lng == 'zh_CN':
#         return 'zh'
#     return lng
