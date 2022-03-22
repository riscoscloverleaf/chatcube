import uuid
import os
import binascii
import logging
import pytz

# from ik.utils.phpserialize import load as phpsess_load
from .models import AuthorizedDevice
from ik.models import Member

logger = logging.getLogger("cc")
RANDOM_STR_LENGTH = 15

def generate_token(member):
    return binascii.hexlify(os.urandom(RANDOM_STR_LENGTH)).decode() + member.get_session_auth_hash()

def make_authorized_device_and_get_new_token(member, data, request):
    token = generate_token(member)

    platform = data.get('platform','')
    tz = get_app_timezone(data.get('timezone'), data.get('utc_offset',0))
    dev = AuthorizedDevice.objects.create(member=member,
                                          platform=platform,
                                          token=token,
                                          timezone=tz,
                                          app_version=data.get('app_version'))

    request.auth = dev
    request.user = dev.member
    return dev



def make_system_api_token():
    if not AuthorizedDevice.objects.filter(member=1,
                                           model=AuthorizedDevice.AUTH_SYSTEM_MODEL,
                                           platform=AuthorizedDevice.PLATFORM_SYSTEM).exists():
        mem = Member.objects.get(pk=1)
        dev = AuthorizedDevice(platform=AuthorizedDevice.PLATFORM_SYSTEM,
                               model=AuthorizedDevice.AUTH_SYSTEM_MODEL,
                               member=mem)
        dev.save()


def get_system_api_token(site_code=None):
    if not site_code:
        site_code = 'default'
    dev = AuthorizedDevice.objects.using(site_code).get(member=1,
                                                        model=AuthorizedDevice.AUTH_SYSTEM_MODEL,
                                                        platform=AuthorizedDevice.PLATFORM_SYSTEM)
    return dev.token

def get_app_timezone(tz, utc_offset):
    zone = None
    if tz:
        if tz[:3] == "GMT":
            tz = "Etc/"+tz
        try:
            zone = pytz.timezone(tz)
        except pytz.UnknownTimeZoneError:
            pass

    if not zone and utc_offset:
        utc = int(int(utc_offset) / 3600)
        if utc >= 0:
            zone = "Etc/GMT+{}".format(utc)
        else:
            zone = "Etc/GMT-{}".format(utc)

    if not zone:
        zone = 'UTC'

    return zone
