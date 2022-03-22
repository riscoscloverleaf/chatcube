from django.conf import settings
from django.utils.encoding import force_text
from django.utils.six.moves.urllib.parse import urlsplit
from django.http import QueryDict
from ik.utils.globals import current_request

def build_absolute_uri(location, request=None, get_params={}):
    location = force_text(location)
    if request is None:
        request = current_request()

    if request is None:
        bits = urlsplit(location)
        if not (bits.scheme and bits.netloc):
            uri = '{proto}://{domain}{url}'.format(
                proto=settings.DEFAULT_HTTP_PROTOCOL,
                domain=settings.API_DOMAIN,
                url=location)
        else:
            uri = location
    else:
        uri = request.build_absolute_uri(location)

    if get_params:
        qd = QueryDict(mutable=True)
        qd.update(**get_params)
        return '?'.join((uri, qd.urlencode()))
    else:
        return uri
