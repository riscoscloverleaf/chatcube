import json

from django.conf import settings
from django.db import models
from django.utils.translation import ugettext_lazy as _
from .members import Member

class MemberSettingsManager(models.Manager):
    use_for_related_fields = True

    def delete_value(self, name, member=None):
        if member is None:
            self.get_queryset().filter(name=name).delete()
        else:
            self.filter(name=name, member=member).delete()

    def set_value(self, name, value, member=None):
        if member is None:
            member = self.instance
            # member = getattr(self, 'instance')

        self.update_or_create(defaults={'data': json.dumps(value, ensure_ascii=False)}, member=member, name=name)

    def get_value(self, name, default=None, member=None):
        if member is None:
            filter_kw = {'name': name}
        else:
            filter_kw = {'name': name, 'member': member}

        try:
            prop = self.values_list('data', flat=True).get(**filter_kw)
            return json.loads(prop)
        except Exception as ex:
            return default



class MemberSettings(models.Model):
    member = models.ForeignKey(settings.AUTH_USER_MODEL, related_name="settings", on_delete=models.CASCADE)
    name = models.CharField(max_length=32)
    data = models.CharField(max_length=16000)

    objects = MemberSettingsManager()

    @property
    def value(self):
        return json.loads(self.data)

    @value.setter
    def value(self, val):
        self.data = json.dumps(val, ensure_ascii=False)
        self.save()

    class Meta:
        db_table = 'member_settings'
        unique_together = (('member', 'name'),)

