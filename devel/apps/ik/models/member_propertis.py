import logging

from django.conf import settings
from django.db import models
from django.utils.translation import get_language, to_locale
from easy_cache import ecached
from ik.utils.ip import ip2location, current_ip_location

logger = logging.getLogger("cc")


class CountryManager(models.Manager):
    @ecached("ccode:{code}" + settings.CACHE_VERSION_TAG, timeout=86400 * 365)
    def get_by_code(self, code):
        return self.filter(code=code).first()

    def get_by_ip(self, ip=None):
        if not ip:
            loc = current_ip_location()
        else:
            loc = ip2location(ip)
        if loc:
            return self.get_by_code(loc.country_short)
        else:
            return None

    def choices_for_lookup(self, ids=None):
        lng = get_language()
        if not lng:
            lng = 'en'
        else:
            lng = to_locale(lng)
        if ids:
            return self.filter(pk__in=ids).order_by(lng)
        else:
            return self.all().order_by(lng)


class Country(models.Model):
    code = models.CharField(primary_key=True, max_length=2)
    region = models.SmallIntegerField()
    language = models.CharField(max_length=3)
    currency = models.CharField(max_length=3)
    phone_code = models.CharField(max_length=5)  # Field name made lowercase.
    timezone = models.IntegerField()
    de = models.CharField(max_length=60)
    en = models.CharField(max_length=60)
    ru = models.CharField(max_length=60)
    pl = models.CharField(max_length=60)
    es = models.CharField(max_length=60)
    it = models.CharField(max_length=60)
    fr = models.CharField(max_length=60)
    th = models.CharField(max_length=60)
    ja = models.CharField(max_length=255, db_column='jp', default='')
    tr = models.CharField(max_length=60)
    pt = models.CharField(max_length=60)
    zh_CN = models.CharField(max_length=255, db_column='zh', default='')
    ko = models.CharField(max_length=60)

    objects = CountryManager()
    @property
    def name(self):
        lng = get_language()
        if not lng:
            lng = 'en'
        if lng == 'zh':
            lng = 'zh_CN'
        return getattr(self, to_locale(lng))

    def __unicode__(self):
        return self.name


    def save(self, force_insert=False, force_update=False, using=None,
             update_fields=None):

        super(Country, self).save(force_insert=force_insert, force_update=force_update, using=using, update_fields=update_fields)

        Country.objects.get_by_code.invalidate_cache_key(code=self.code)

    def delete(self, using=None, keep_parents=False):
        Country.objects.get_by_code.invalidate_cache_key(code=self.code)

        super(Country, self).delete(using=using, keep_parents=keep_parents)

    class Meta:
        ordering = ('en',)
        managed = False
        db_table = 'countries'
