import logging
from random import randint
from django.conf import settings
from django.contrib.auth import authenticate
from django.utils.translation import gettext as _
from rest_framework import exceptions
from rest_framework import fields
from rest_framework import serializers

from ik import constants
from ik.account_adapter import IKAccountAdapter
from ik.models.members import Member, MemberLoginCode
from ik.utils.iklocale import date_time_since
logger = logging.getLogger("cc")

class LoginSerializer(serializers.Serializer):
    email = serializers.CharField(required=True, allow_blank=False)
    password = serializers.CharField(required=True, allow_blank=False)

    platform = serializers.CharField(required=False, allow_blank=True)
    app_version = serializers.CharField(required=False, allow_blank=True)

    def validate(self, attrs):
        email = attrs.get('email')
        password = attrs.get('password')

        if email and password:
            user = authenticate(email=email, password=password)
        else:
            msg = _('Must include "email" and "password".')
            raise exceptions.ValidationError(msg)

        # Did we get back an active user?
        if user:
            if not user.is_active:
                msg = _('User account is disabled.')
                raise exceptions.ValidationError(msg)
        else:
            msg = _('Unable to log in with provided credentials.')
            raise exceptions.ValidationError(msg)

        attrs['user'] = user
        return attrs


class SignupSerializer(serializers.ModelSerializer):
    password = fields.CharField(required=True)
    email = fields.CharField(required=True)
    displayname = fields.CharField(required=True)

    platform = serializers.CharField(required=False, allow_blank=True)
    app_version = serializers.CharField(required=False, allow_blank=True)
    newsletter = serializers.BooleanField(required=False, default=False)

    def validate_email(self, email):
        return IKAccountAdapter.clean_email(email)

    def validate_phone(self, phone):
        if 'request' in self.context:
            country_code = self.context['request'].data.get('country_code')
        else:
            country_code = None

        return IKAccountAdapter.clean_phone(phone, country_code=country_code)

    def validate_userid(self, userid):
        return IKAccountAdapter.clean_userid(userid)

    def validate_app_version(self, ver):
        if ver:
            parts = ver.split("\n")
            return parts[0].strip()
        else:
            return None

    def create(self, validated_data):
        member = IKAccountAdapter.save_user(self.context['request'], Member(), validated_data)
        if validated_data['newsletter']:
            member.settings.set_value("newsletter", 1)
        else:
            member.settings.set_value("newsletter", 0)

        return member

    class Meta:
        model = Member
        fields = ('email', 'phone', 'userid', 'first_name', 'last_name', 'displayname', 'password',
                  'platform', 'riscos', 'app_version', 'newsletter')


class ChangePasswordSerializer(serializers.Serializer):
    old_password = serializers.CharField(required=False)
    password = serializers.CharField()

    def validate_old_password(self, value):

        if not self.instance.check_password(value):
            raise serializers.ValidationError(_("Incorrect old password. Please go back and retype it!"))
        return value

    def validate(self, attrs):
        pw = attrs['password']
        if self.instance.password and 'old_password' in attrs and attrs['old_password'] == pw:
            raise serializers.ValidationError(_("Don`t use the same password again or password content has incorrect chars"))

        if len(pw) < settings.ACCOUNT_PASSWORD_MIN_LENGTH:
            raise serializers.ValidationError(
                _("Wrong password. Password must be {0} - {1} characters").format(settings.ACCOUNT_PASSWORD_MIN_LENGTH, 30))

        return attrs


class VerifyCodeSerializer(serializers.Serializer):
    code = serializers.CharField(required=True, allow_blank=False)

    email = serializers.CharField(allow_blank=True, required=False)
    phone = serializers.CharField(allow_blank=True, required=False)

    def validate(self, attrs):
        code = attrs.get('code')
        phone = attrs.get('phone')
        email = attrs.get('email')

        if not phone and not email:
            raise serializers.ValidationError(_("Either 'email' or 'phone' must be set"))

        if phone:
            mem = IKAccountAdapter.get_member_by_phone(phone)
        else:
            mem = IKAccountAdapter.get_member_by_email(email)

        code_obj = MemberLoginCode.objects.filter(member=mem, code=code).first()
        if not code_obj:
            raise exceptions.ValidationError(_("Invalid verification code. Please check for typing errors"))

        if code_obj.used_at:
            raise exceptions.ValidationError(_("Code already used {0}").format(date_time_since(code_obj.used_at)))

        if not mem.is_active:
            msg = _('User account is disabled.')
            raise exceptions.ValidationError(msg)

        logger.info("Verified code. code: %s, member: %s", code, mem)
        attrs['user'] = mem
        attrs['code_obj'] = code_obj
        return attrs

class SetPasswordSerializer(VerifyCodeSerializer):
    password1 = serializers.CharField()
    password2 = serializers.CharField(required=False)

    def validate(self, attrs):
        attrs = super(SetPasswordSerializer, self).validate(attrs)
        pw1 = attrs['password1']
        pw2 = attrs['password2']
        if pw1 != pw2:
            raise serializers.ValidationError(_("Passwords don't match"))

        return attrs
