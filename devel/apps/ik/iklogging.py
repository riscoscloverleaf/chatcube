import logging
from django.conf import settings
from django.utils.log import AdminEmailHandler
from django.http.request import HttpRequest
from ik.utils.ip import current_ip
from ik.utils.globals import current_user, current_request

class RequireAppTestModeTrue(logging.Filter):
    def filter(self, record):
        return settings.APP_TEST_MODE


class RequireAppTestModeFalse(logging.Filter):
    def filter(self, record):
        return not settings.APP_TEST_MODE

class RequireDebugDb(logging.Filter):
    def filter(self, record):
        return hasattr(record, 'debugdb')


class IPUserFormatter(logging.Formatter):

    def __init__(self, **kwargs):
        fmt = kwargs.get('format', None)
        dfmt = kwargs.get('datefmt', None)
        super(IPUserFormatter, self).__init__(fmt, dfmt)

    def format(self, record):
        record.user = '-'
        record.url = "-"
        record.ip_address = '0.0.0.0'

        request = current_request()
        if request:
            try:
                record.url = request.build_absolute_uri()
            except:
                pass
            if hasattr(request, '_cached_user') and request._cached_user.is_authenticated:
                # hack but if try to get request.user which is Lazy object then every logging SQL may query database again
                record.user = "{}:{}".format(request._cached_user.id, request._cached_user.email)
            elif hasattr(request, 'auth') and request.user.is_authenticated: # rest mobile app auth case
                record.user = "API {}:{} {}".format(request.user.id, request.user.email, request.auth.app_version)

            record.ip_address = current_ip()

        return super(IPUserFormatter, self).format(record)


class IKAdminEmailHandler(AdminEmailHandler):
    def emit(self, record):
        if not hasattr(record, "request"):
            request = current_request()
            if request:
                record.request = request
            else:
                record.request = HttpRequest()
                record.request.META['SERVER_NAME'] = 'localhost'
                record.request.META['SERVER_PORT'] = 0
                record.request.META['REMOTE_ADDR'] = '127.0.0.1'
        super(IKAdminEmailHandler, self).emit(record)

