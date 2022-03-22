from django.conf import settings
from django.utils import timezone
from django.contrib import auth
from ipware.ip import get_real_ip

from ik.signals import ip_addess_changed
from ik.models.members import Member

#LAST_ONLINE_UPDATE_FREQ = 30
#SESSION_IP_ADDR = "ipaddr"

# class DatingAdminAuthMiddleware(object):
#     # DatingAdminAuthMiddleware must be placed BEFORE AuthenticationMiddleware
#     def process_request(self, request):
#         if request.path[:len(settings.ADMIN_ROOT_URL)] == settings.ADMIN_ROOT_URL:
#             try:
#                 username = request.META.get('REMOTE_USER', request.META.get('HTTP_REMOTE_USER'))
#             except KeyError:
#                 return
#
#             # We are seeing this user for the first time in this session, attempt
#             # to authenticate the user.
#             user = auth.authenticate(remote_user=username)
#             if user:
#                 # User is valid.  Set request.user and persist user in the session
#                 # by logging the user in.
#                 request._cached_user = user


