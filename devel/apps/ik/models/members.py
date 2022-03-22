import os
import uuid
import logging
import phonenumbers
import hashlib
import time
from random import randint
from datetime import datetime, timedelta

from django.conf import settings
from django.contrib import auth
from django.contrib.auth.models import BaseUserManager, AbstractBaseUser, Group
from django.core.exceptions import PermissionDenied
from django.db import models
from django.utils.functional import cached_property
from django.utils.translation import ugettext_lazy as _
from django.utils.timezone import now
from easy_thumbnails.fields import ThumbnailerImageField
from easy_thumbnails.files import ThumbnailerFieldFile
from encrypted_fields.fields import EncryptedCharField, SearchField
from ik import constants
from ik.utils.site import build_absolute_uri
from ik.utils.storage import UUIDPath
from ik.fields.unsignedauto import UnsignedAutoField
from ik.signals import member_online_changed
from .member_propertis import Country
from ik.signals import member_online_changed, member_active_changed
from . import member_choices

logger = logging.getLogger("cc")

# blocks_cache_prefix= "m:{self.id}:blocks"+settings.CACHE_VERSION_TAG
# contacts_cache_prefix= "m:{self.id}:contacts"+settings.CACHE_VERSION_TAG
# favorites_cache_prefix = "m:{self.id}:favorites"+settings.CACHE_VERSION_TAG
# photos_cache_prefix= "m:{self.id}:photos"+settings.CACHE_VERSION_TAG

_default_profile_photo = "default/photo_profile.png"
_default_notification_setting = {'popup': False, 'taskbar': False, 'sound': False, 'unread_age': 48}

class MemberManager(BaseUserManager):
    def create_user(self, email, password=None):
        """
        Creates and saves a User with the given email and password.
        """
        if not email:
            raise ValueError('Users must have an email address')

        user = self.model(
            email=self.normalize_email(email),
        )

        user.set_password(password)
        user.save(using=self._db)
        return user

    def create_superuser(self, email, password):
        """
        Creates and saves a superuser with the given email and password.
        """
        user = self.create_user(email,
            password=password
        )
        user.save(using=self._db)
        return user


    def get_by_natural_key(self, key):
        return self.get(email=key)

    def filter_by_email(self, email):
        # ehash = field_hash(email)
        # return self.filter(email_hash=ehash)
        return self.filter(email=email)

    def filter_by_phone(self, phone):
        # phone_hash = field_hash(phone)
        # return self.filter(phone_hash=phone_hash)
        return self.filter(phone=phone)

    def filter_by_userid(self, userid):
        # phone_hash = field_hash(phone)
        # return self.filter(phone_hash=phone_hash)
        return self.filter(userid=userid)

    def get_member(self, id):
        return self.filter(pk=id).first()

    def get_member_by_hash(self, val):
        hash_with_id = val[20:]
        end_id = hash_with_id.index("f")
        id = int(hash_with_id[:end_id])
        member = self.get(pk=id)
        if member.get_member_hash() == val:
            return member
        raise member.DoesNotExist("Member with specified hash not found")

    def generate_push_channel_name(self):
        channel = uuid.uuid4().hex
        while self.filter(push_channel=channel).exists():
            logging.info("Push channel {} already exists. generating new name".format(channel))
            channel = uuid.uuid4().hex

        return channel

class Member(AbstractBaseUser):
    is_anonymous = False
    is_authenticated = True

    id = UnsignedAutoField(primary_key=True)

    _email_data = EncryptedCharField(max_length=128, editable=False)
    email = SearchField(hash_key='3c8d901b1fd99e7c5238df8205e57c0c',
                        unique=True, verbose_name='Email', encrypted_field_name="_email_data")

    userid = models.CharField(max_length=16, default='', verbose_name='UserID', db_index=True)

    first_name = models.CharField(max_length=100, blank=True, null=True, verbose_name='First name')
    last_name = models.CharField(max_length=100, blank=True, null=True, verbose_name='Last name')
    displayname = models.CharField(max_length=100, verbose_name=_('Display name'))

    _phone_data = EncryptedCharField(max_length=22, editable=False, blank=True, null=True)
    phone = SearchField(hash_key='5998b869dbeffe7daf52794ded2f9a91',
                        verbose_name='Phone', encrypted_field_name="_phone_data")

    password = models.CharField(max_length=128, default='', verbose_name=_("Password"))
    profile_image = ThumbnailerImageField(upload_to=UUIDPath('profile'),
                                          max_length=255, default='', blank=True)
    riscos = models.CharField(max_length=64, default='', verbose_name='RiscOS platform')

    country = models.ForeignKey(Country, null=True, blank=True, on_delete=models.SET_NULL)
    city = models.CharField(max_length=255, default='', blank=True)
    website = models.CharField(max_length=255, default='', blank=True)

    date_joined = models.DateTimeField(auto_now_add=True)

    is_active = models.BooleanField(default=True)
    is_staff = models.BooleanField(default=False)

    push_channel = models.CharField(max_length=34, unique=True)

    groups = models.ManyToManyField(
        Group,
        verbose_name=_('groups'),
        blank=True,
        help_text=_(
            'The groups this user belongs to. A user will get all permissions '
            'granted to each of their groups.'
        ),
        related_name="user_set",
        related_query_name="user",
    )

    USERNAME_FIELD = 'email'
    REQUIRED_FIELDS = []

    objects = MemberManager()

    @property
    def is_superuser(self):
        return self.email == 'admin@chatcube.org'

    @property
    def profile_photo(self):
        if self.profile_image.name == '':
            return ThumbnailerFieldFile(self, self.profile_image, _default_profile_photo)
        try:
            if self.profile_image.width == 0 or self.profile_image.height == 0:
                logger.warning(u"Profile image [{}] missing or corrupted".format(self.profile_image.name))
                return ThumbnailerFieldFile(self, self.profile_image, _default_profile_photo)
        except Exception as ex:
            logger.warning(u"Profile image [{}] missing or corrupted. Error: {}".format(self.profile_image.name, ex))
            return ThumbnailerFieldFile(self, self.profile_image, _default_profile_photo)
        return self.profile_image

    @cached_property
    def pic_small(self):
        return build_absolute_uri(self.profile_photo.get_thumbnail(settings.SMALL_PROFILE_THUMBNAIL_CONF).url)

    @cached_property
    def pic_medium(self):
        return build_absolute_uri(self.profile_photo.get_thumbnail(settings.ATTACHMENT_THUMBNAIL_CONF).url)

    @cached_property
    def pic(self):
        return build_absolute_uri(self.profile_photo.url)

    def get_full_name(self):
        """
        Returns the first_name plus the last_name, with a space in between.
        """
        return self.displayname

    def get_short_name(self):
        "Returns the short name for the user."
        return self.displayname


    def get_group_permissions(self, obj=None):
        """
        Returns a list of permission strings that this user has through their
        groups. This method queries all available auth backends. If an object
        is passed in, only permissions matching this object are returned.
        """
        permissions = set()
        for backend in auth.get_backends():
            if hasattr(backend, "get_group_permissions"):
                permissions.update(backend.get_group_permissions(self, obj))
        return permissions

    def get_all_permissions(self, obj=None):
        permissions = set()
        for backend in auth.get_backends():
            if hasattr(backend, "get_all_permissions"):
                permissions.update(backend.get_all_permissions(self, obj))
        return permissions

    def has_perm(self, perm, obj=None):
        """
        Returns True if the user has the specified permission. This method
        queries all available auth backends, but returns immediately if any
        backend returns True. Thus, a user who has permission from a single
        auth backend is assumed to have permission in general. If an object is
        provided, permissions for this specific object are checked.
        """

        # Active superusers have all permissions.
        if not self.is_active:
            return False

        if self.is_superuser:
            return True

        for backend in auth.get_backends():
            if not hasattr(backend, 'has_perm'):
                continue
            try:
                if backend.has_perm(self, perm, obj):
                    return True
            except PermissionDenied:
                return False
        return False

    def has_perms(self, perm_list, obj=None):
        """
        Returns True if the user has each of the specified permissions. If
        object is passed, it checks if the user has all required perms for this
        object.
        """
        return all(self.has_perm(perm, obj) for perm in perm_list)

    def has_module_perms(self, app_label):
        """
        Returns True if the user has any permissions in the given app label.
        Uses pretty much the same logic as has_perm, above.
        """
        # Active superusers have all permissions.
        if self.is_staff:
            return True

        for backend in auth.get_backends():
            if not hasattr(backend, 'has_module_perms'):
                continue
            try:
                if backend.has_module_perms(self, app_label):
                    return True
            except PermissionDenied:
                return False
        return False


    def notification_settings(self):
        return self.settings.get_value('notification_settings', _default_notification_setting)

    def set_notification_settings(self, notification_settings):
        settings = self.notification_settings()
        settings.update(notification_settings)
        self.settings.set_value('notification_settings', settings)

    def get_member_hash(self):
        digest = hashlib.sha256(self.email.encode('utf8')).hexdigest()
        return digest[:20] + str(self.id) + 'f' + digest[20:]

    def as_dict(self, me):
        is_me = me.id == self.id
        result = {
            'id': constants.MESSENGER_CHATCUBE+str(self.id),
            'displayname': self.displayname,
            'userid': self.userid,
            'first_name': self.first_name,
            'last_name': self.last_name,
            'phone': self.phone if is_me else '',
            'city': self.city,
            'country': self.country_id,
            'website': self.website,
            'pic': self.pic,
            'pic_small': self.pic_small,
            'email': self.email if is_me else '',
            'date_joined': int(self.date_joined.timestamp())
        }
        result.update(self.online.as_dict(me))
        return result

    def mark_as_deleted(self):
        logger.info("Member id:%s [%s] deleted", self.id, self.__str__())

        self.is_active = False
        self.first_name = "member deleted"
        self.displayname = "member deleted"
        self.email = "deleted-" +uuid.uuid4().hex
        self.last_name = ""
        self.city = ""
        self.phone = ""
        self.website = ""
        self.userid = ""
        self.profile_image = ""
        self.riscos = ""
        self.country = None
        self.set_unusable_password()
        self.save()

    def __str__(self):
        return u"{} <{}>".format(self.displayname, self.email)


    class Meta:
        verbose_name = 'Member'
        verbose_name_plural = 'Members'
        db_table = 'members'


class MemberOnline(models.Model):
    INACTIVE_TIMEOUT = 240
    member = models.OneToOneField(settings.AUTH_USER_MODEL, related_name='online', primary_key=True,
                                  on_delete=models.CASCADE)
    online = models.BooleanField(default=False)
    was_online = models.DateTimeField(auto_now_add=True)
    last_action = models.DateTimeField(auto_now_add=True)
    active = models.BooleanField(default=False)

    def set_online(self, action=False):
        t_now = now()
        self.was_online = t_now
        if not self.online:
            self.online = True
            self.active = True
            self.last_action = t_now
            self.save()
            member_online_changed.send(self.__class__, online=self)
        else:
            if action:
                self.last_action = t_now
                if not self.active:
                    self.active = True
                    member_active_changed.send(self.__class__, online=self)
            else:
                if self.active and self.last_action and (t_now - self.last_action).seconds > self.INACTIVE_TIMEOUT:
                    self.active = False
                    member_active_changed.send(self.__class__, online=self)

            self.save()

    def set_offline(self):
        if self.online:
            self.online = False
            self.active = False
            self.save()
            member_online_changed.send(self.__class__, online=self)

    def as_dict(self, me):
        result = {
            'id': constants.MESSENGER_CHATCUBE + str(self.member_id),
            'online': self.online,
            'was_online': int(self.was_online.timestamp()),
            'last_action': int(self.last_action.timestamp()),
            'active': self.active
        }

        return result

    class Meta:
        verbose_name = 'Member online'
        verbose_name_plural = 'Members online'
        db_table = 'members_online'

def autocreate_online(sender, instance=None, created=False, **kwargs):
    if created:
        MemberOnline.objects.create(member=instance)

models.signals.post_save.connect(autocreate_online, sender=Member)


class MemberLoginCodeManager(models.Manager):
    # def verify_code(self, member, code):
    #     rec = MemberLoginCode.objects.filter(member=member, code=code).first()
    #     if rec:
    #         rec.verified_at = now()
    #         rec.save()
    #
    #     return rec

    def make_code(self, member):
        code = randint(100000, 999999)

        dt = now()
        code_regeneration_age = dt - timedelta(seconds=3600)

        code_obj, _created = self.get_or_create(member=member, defaults={
            'code': code,
            'created_at': dt
        })

        if code_obj.created_at < code_regeneration_age:
            code_obj.code = code

        code_obj.created_at = now()
        code_obj.verified_at = None
        code_obj.save()

        return code_obj


class MemberLoginCode(models.Model):
    member = models.OneToOneField(settings.AUTH_USER_MODEL, related_name='+', primary_key=True, on_delete=models.CASCADE)
    code = models.CharField(max_length=8, default='')
    created_at = models.DateTimeField(null=True)
    used_at = models.DateTimeField(null=True)

    objects = MemberLoginCodeManager()

    class Meta:
        db_table = 'member_login_code'

