import logging

from django import http
from django.utils import timezone
from django.utils.cache import patch_vary_headers
from rest_framework.authentication import get_authorization_header
from ipware.ip import get_real_ip
from .models import AuthorizedDevice
from ik.signals import ip_addess_changed

logger = logging.getLogger("cc")

class IKAPIMiddleware(object):
    # DatingAPITokenAuthMiddleware must be placed BEFORE AuthenticationMiddleware
    keyword = 'Token'
    post_field = 'XPostAuth'

    def __init__(self, get_response):
        self.get_response = get_response

    def __call__(self, request):
        self.process_request(request)
        return  self.get_response(request)

    def process_request(self, request):
        # If CORS preflight header, then create an
        # empty body response (200 OK) and return it
        if (request.method == 'OPTIONS' and 'HTTP_ACCESS_CONTROL_REQUEST_METHOD' in request.META):
            return http.HttpResponse()

        # try to authenticate the user by token
        auth = get_authorization_header(request).split()

        # special case that auth token can be passed in POST request
        if not auth and request.method == 'POST':
            auth = request.POST.get(self.post_field, '').split()

        if not auth or auth[0].lower() != self.keyword.lower().encode():
            return

        if len(auth) == 1:
            logger.warning(u'Invalid token header. No credentials provided.')
            return
        elif len(auth) > 2:
            logger.warning(u'Invalid token header. Token string should not contain spaces.')
            return

        try:
            token = auth[1].decode()
        except UnicodeError:
            logger.warning(u"Invalid token header. Token string should not contain invalid characters.")
            return

        try:
            update_fields = []
            dev = AuthorizedDevice.objects.select_related('member','member__online').get(token=token)
            request.auth = dev
            request.user = dev.member
            if dev.timezone:
                try:
                    timezone.activate(dev.timezone)
                except:
                    logger.error("Failed to switch to time zone [{}]".format(dev.timezone))

            now = timezone.now()
            timediff = now - dev.was_online
            real_ip = get_real_ip(request)
            old_ip = dev.ip_address
            if old_ip != real_ip or timediff.seconds > 60 or not dev.member.online.active or not dev.member.online.online:
                update_fields = ['was_online']
                dev.was_online = now
                dev.member.online.set_online(action=True)
                if old_ip != real_ip:
                    dev.ip_address = real_ip
                    ip_addess_changed.send(request.user.__class__, request=request, old_ip=old_ip, new_ip=real_ip)
                    update_fields.append('ip_address')

            if len(update_fields) > 0:
                dev.save(update_fields=update_fields)

        except AuthorizedDevice.DoesNotExist:
            logger.warning(u"AuthorizedDevice does not exists for token: {}".format(token))
            return
