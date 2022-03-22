import pickle
from datetime import timedelta
from django.utils.timezone import now
from django.core.exceptions import ObjectDoesNotExist
from django.db import models
from django.db.models import Q

class GlobalVarsManager(models.Manager):

    def delete_value(self, name):
        self.get_queryset().filter(name=name).delete()

    def set_value(self, name, value, timeout=None):
        if timeout:
            valid_to = now() + timedelta(seconds=timeout)
        else:
            valid_to = None

        self.update_or_create(defaults={'value': pickle.dumps(value,0), 'valid_to': valid_to}, name=name)

    def get_value(self, name, default=None):
        prop = self.filter(Q(name=name)&(Q(valid_to__gt=now())|Q(valid_to=None))).values_list('value', flat=True).first()
        if prop:
            try:
                return pickle.loads(str(prop))
            except Exception:
                return default
        else:
            return default


class GlobalVars(models.Model):
    name = models.CharField(max_length=255, primary_key=True)
    value = models.CharField(max_length=16000)
    valid_to = models.DateTimeField()

    objects = GlobalVarsManager()

    class Meta:
        db_table = 'global_vars'
