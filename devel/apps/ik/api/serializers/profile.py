import logging
import time
from email.utils import formatdate
from collections import OrderedDict
from rest_framework import fields
from rest_framework import serializers
from django.core.exceptions import ObjectDoesNotExist
from django.core.files.storage import default_storage
from ik.models.members import Member
from ik.account_adapter import IKAccountAdapter
from ik.signals import member_changed
from .telegram import MyProfileTelegramAccountSerializer
from ik.messengers.telegram.client import IKTelegramClient
from ik import constants

logger = logging.getLogger("cc")


class MemberSettingsField(fields.Field):

    def __init__(self, setting_name, **kwargs):
        super(MemberSettingsField, self).__init__(**kwargs)
        self.setting_name = setting_name

    def to_representation(self, obj):
        return obj.settings.get_value(self.setting_name)

    def to_internal_value(self, data):
        return {self.setting_name: data}


class NotificationSettingsSerializer(serializers.Serializer):
    popup = serializers.BooleanField(required=False)
    taskbar = serializers.BooleanField(required=False)
    sound = serializers.BooleanField(required=False)
    unread_age = serializers.IntegerField(required=False)


class MyProfileSerializer(serializers.ModelSerializer):
    id = serializers.SerializerMethodField()
    avatar = serializers.CharField(required=False, write_only=True)

    notification_settings = NotificationSettingsSerializer()
    telegram_account = MyProfileTelegramAccountSerializer()

    push_channel_start_date = serializers.SerializerMethodField()

    # def get_my_telegram_profile(self, instance):
    #     if not hasattr(self, "_telegram_account"):
    #         client = IKTelegramClient.get_client(instance, False)
    #         if client is None:
    #             self._telegram_account = {}
    #         else:
    #             self._telegram_account = client.get_me()
    #     return self._telegram_account

    def get_id(self, instance):
        return constants.MESSENGER_CHATCUBE+str(instance.id)

    def get_push_channel_start_date(self, instance):
        return formatdate(time.time() - 1, usegmt=True)

    def validate_email(self, email):
        return IKAccountAdapter.clean_email(email, exclude_me=self.instance)

    def validate_phone(self, phone):
        if 'request' in self.context:
            country_code = self.context['request'].data.get('country_code')
        else:
            country_code = None

        return IKAccountAdapter.clean_phone(phone, country_code=country_code, exclude_me=self.instance)

    # def get_telegram_account__first_name(self, instance):
    #     profile = self.get_my_telegram_profile(instance)
    #     return profile.get('first_name','')
    #
    # def get_telegram_account__last_name(self, instance):
    #     profile = self.get_my_telegram_profile(instance)
    #     return profile.get('last_name','')
    #
    # def get_telegram_account__username(self, instance):
    #     profile = self.get_my_telegram_profile(instance)
    #     return profile.get('userid','')
    #
    # def get_telegram_account__user_id(self, instance):
    #     profile = self.get_my_telegram_profile(instance)
    #     return profile.get('user_id',0)
    #
    # def get_telegram_account__pic(self, instance):
    #     profile = self.get_my_telegram_profile(instance)
    #     return profile.get('pic','')

    def validate_userid(self, userid):
        return IKAccountAdapter.clean_userid(userid, exclude_me=self.instance)

    def get_fields(self):
        fields = super(MyProfileSerializer, self).get_fields()
        only_fields = self.context.get('only',[])
        if len(only_fields) > 0:
            new_fields = OrderedDict()
            for f in fields:
                if f in only_fields:
                    new_fields[f] = fields[f]
            return new_fields
        else:
            return fields

    def update(self, instance, validated_data):
        notification_settings = validated_data.pop('notification_settings', None)
        if notification_settings:
            instance.set_notification_settings(notification_settings)

        avatar = validated_data.pop('avatar', None)
        if avatar and default_storage.exists(avatar):
            instance.profile_image = avatar
            instance.save()

        telegram_account_data = validated_data.pop("telegram_account", None)
        if telegram_account_data:
            self.fields['telegram_account'].update(instance.telegram_account, telegram_account_data)

        result = super().update(instance, validated_data)

        member_changed.send(Member, member=instance)
        return result

    class Meta:
        model = Member
        fields = ('id', 'displayname', 'userid', 'first_name', 'last_name', 'phone', 'city', 'country', 'website',
                  'pic', 'pic_small', 'online',
                  'profile_image', 'pic_medium', 'email', 'avatar', 'city', 'country',
                  'push_channel', 'push_channel_start_date',
                  'telegram_account',
                  # 'telegram_account__phone', 'telegram_account__first_name', 'telegram_account__last_name',
                  # 'telegram_account__username', 'telegram_account__user_id',
                  # 'telegram_account__pic', 'telegram_account__avatar', 'telegram_account__profile_image',
                  'notification_settings',
                  # 'notification_settings__popup', 'notification_settings__taskbar', 'notification_settings__sound',
                  )
        read_only_fields = ('date_joined','online', 'id')
