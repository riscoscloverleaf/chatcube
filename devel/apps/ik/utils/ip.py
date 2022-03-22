import IP2Location, socket, struct
from django.conf import settings
from ipware.ip import get_real_ip

from ik.utils.globals import current_request


def ip2location(ip):
    if ":" in ip:
        IP2LocObj = IP2Location.IP2Location(settings.IPV62LOCATION_DB)
    else:
        IP2LocObj = IP2Location.IP2Location(settings.IP2LOCATION_DB)

    loc = IP2LocObj.get_all(ip)
    if loc and loc.country_short == 'GB':
        loc.country_short = 'UK'
    return loc


def current_ip_location(request=None):
    return ip2location(current_ip(request))


def ip2long(ip):
    """
    Convert an IP string to long
    """
    try:
        packedIP = socket.inet_aton(ip)
        return struct.unpack("!L", packedIP)[0]
    except:
        # probably IP6 address. Can't converted
        packedIP = socket.inet_aton('0.0.0.0')
        return struct.unpack("!L", packedIP)[0]


def current_ip(request=None):
    if not request:
        request = current_request()
    if request:
        ip = get_real_ip(request)
        if not ip:
            ip = '127.0.0.1'
    else:
        ip = '0.0.0.0'
    return ip