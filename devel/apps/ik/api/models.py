import binascii
import logging
import os
from django.conf import settings
from django.db import models

logger = logging.getLogger("cc")

RANDOM_STR_LENGTH = 32

class AuthorizedDevice(models.Model):
    AUTH_SYSTEM_MODEL = "SystemAPI"
    PLATFORM_RISCOS = 'RiscOS'
    PLATFORM_SYSTEM = 'System'

    id = models.BigAutoField(primary_key=True)
    member = models.ForeignKey(settings.AUTH_USER_MODEL, related_name='devices', on_delete=models.CASCADE)
    token = models.CharField(max_length=255, unique=True)
    platform = models.CharField(max_length=64, blank=True, null=True)
    app_version = models.CharField(max_length=32, blank=True, null=True)
    registered_at = models.DateTimeField(auto_now_add=True)
    was_online = models.DateTimeField(auto_now=True)
    ip_address = models.GenericIPAddressField(blank=True, null=True)
    timezone = models.CharField(max_length=255, blank=True, null=True)

    def __str__(self):
        return "{} {} ".format(self.member, self.token)

    def __unicode__(self):
        return u"{} {} ".format(self.member, self.token)

    def regenerate_token(self):
        self.token = binascii.hexlify(os.urandom(RANDOM_STR_LENGTH)).decode()
        self.save(update_fields=['token'])

    def is_system_api(self):
        return self.platform == self.PLATFORM_SYSTEM

    class Meta:
        db_table = 'authorized_device'

