import logging
import shutil
from django.db import models
from django.conf import settings
from ik.models import Member
from .utils import telegram_files_directory
from ik.utils.site import build_absolute_uri
from encrypted_fields.fields import EncryptedCharField, SearchField

logger = logging.getLogger("cc")

class TelegramAccount(models.Model):
    member = models.OneToOneField(Member, related_name='telegram_account', primary_key=True, on_delete=models.CASCADE)

    _phone_data = EncryptedCharField(max_length=22)
    phone = SearchField(hash_key='21a90509d5098703462a23319f8cec11',
                        verbose_name='Phone', encrypted_field_name="_phone_data", unique=True)

    tg_user_id = models.BigIntegerField(default=0)

    def first_name(self):
        return self.get_tg_profile().get("first_name","")

    def last_name(self):
        return self.get_tg_profile().get("last_name","")

    def username(self):
        return self.get_tg_profile().get("username","")

    def pic(self):
        return self.get_tg_profile().get("pic","")

    def update_my_data(self, first_name, last_name, username, photo, avatar):
        from .client import IKTelegramClient
        client = IKTelegramClient(self)
        tg_profile = self.get_tg_profile()
        if first_name != self.first_name() or last_name != self.last_name():
            client.call_method("setName", first_name=first_name, last_name=last_name)
            tg_profile['first_name'] = first_name
            tg_profile['last_name'] = last_name

        if username != self.username():
            client.call_method("setUsername", username=username)
            tg_profile['username'] = username

        if photo:
            tg_profile['pic'] = client.set_my_profile_photo(photo)
        elif avatar:
            tg_profile['pic'] = client.set_my_avatar(avatar)

    def get_tg_profile(self):
        from .client import IKTelegramClient, TelegramUnauthorizedException
        if not hasattr(self, "_tg_profile"):
            client = IKTelegramClient(self)
            try:
                self._tg_profile = client.get_me()
            except TelegramUnauthorizedException as ex:
                logger.warning("get_tg_profile error: {}".format(ex))
                return {}
        return self._tg_profile

    class Meta:
        db_table = 'telegram_accounts'

def on_telegram_account_delete(sender, instance, **kwargs):
    shutil.rmtree(telegram_files_directory(instance.phone), ignore_errors=True)

models.signals.post_delete.connect(on_telegram_account_delete, sender=TelegramAccount)