import logging

import phonenumbers
from django import forms
from django.conf import settings
from django.core.validators import validate_email
from django.utils.translation import gettext as _

from ik.models import Member
from ik.signals import member_signed_up
from ik.utils.ip import current_ip_location

logger = logging.getLogger("cc")

_email_blacklist_domains = set()

strip_chars = u" \t\r\n\u00A0\u180E\u200B\uFEFF\u3000\u205F\u202F\u200A\u2009\u2008\u2007\u2006\u2005\u2004\u2003\u2002\u2001\u2000"

class IKAccountAdapter(object):
    @classmethod
    def normalize_email(cls, email):
        email = email.strip(strip_chars)
        if not email:
            return ''

        if "@" not in email:
            raise forms.ValidationError(_("The email address [{}] is not correct! Please check for typing errors!").format(email))

        user_part, domain_part = email.rsplit('@', 1)
        domain_part = domain_part.lower()

        if domain_part == "gmali.com":
            domain_part = "gmail.com"

        return "{}@{}".format(user_part, domain_part)

    @classmethod
    def clean_email(cls, email, exclude_me=None):
        email = cls.normalize_email(email)
        if not email:
            return ''

        user_part, domain_part = email.rsplit('@', 1)

        validate_email(email)
        if domain_part == settings.EMAIL_DOMAIN:
            raise forms.ValidationError(_("The email address [{}] is not correct! Please check for typing errors!").format(email))

        if domain_part in _email_blacklist_domains:
            raise forms.ValidationError(_("Sorry we cannot accept email of AT&T domains ({0}, att.net, bellsouth.net, sbcglobal.net, ...) as we get blocked continously by it.")\
                                        .format(domain_part) + " " + _("Please use another email address to register on our site."))

        qs = Member.objects.filter_by_email(email)
        if exclude_me:
            qs = qs.exclude(id=exclude_me.id)
        if qs.exists():
            raise forms.ValidationError(_("A user is already registered"
                                          " with this e-mail address: {}").format(email))
        # if not verify_email_address(email):
        #     raise forms.ValidationError(_("We got response from your email provider that your email doesn't exists") \
        #             + " " + _("Please use another email address to register on our site."))

        return email

    @classmethod
    def normalize_phone(cls, phone, country_code=None):
        try:
            if phone:
                phone = phone.strip(strip_chars)
            else:
                return ''
            if not country_code:
                loc = current_ip_location()
                if loc:
                    country_code = loc.country_short

            if not country_code:
                country_code = None
            num = phonenumbers.parse(phone, region=country_code)
            logger.info("normalize_phone [{}] country_code:[{}] valid:{}".format(phone, country_code, phonenumbers.is_valid_number(num)))
            if phonenumbers.is_valid_number(num):
                return phonenumbers.format_number(num, phonenumbers.PhoneNumberFormat.E164)
        except phonenumbers.phonenumberutil.NumberParseException:
            pass
        raise forms.ValidationError(_("Invalid phone number: {}").format(phone))

    @classmethod
    def clean_phone(cls, phone, country_code=None, exclude_me=None):
        logger.info("clean_phone [{}] country_code:[{}]".format(phone, country_code))

        # needs revert back and comment "phone = phone.strip(strip_chars)" line
        #phone = cls.normalize_phone(phone, country_code)
        if phone:
            phone = phone.strip(strip_chars)
            qs = Member.objects.filter_by_phone(phone)
            if exclude_me:
                qs = qs.exclude(id=exclude_me.id)
            if qs.exists():
                raise forms.ValidationError(_("A user is already registered with this phone number: {}").format(phone))
        return phone

    @classmethod
    def clean_userid(cls, userid, exclude_me=None):
        qs = Member.objects.filter_by_userid(userid)
        if exclude_me:
            qs = qs.exclude(id=exclude_me.id)
        if qs.exists():
            raise forms.ValidationError(_("A user is already registered with this UserID."))
        return userid

    @classmethod
    def get_member_by_phone(cls, phone, country_code=None, exclude_me=None):
        phone = cls.normalize_phone(phone, country_code)
        if phone:
            mem = Member.objects.filter_by_phone(phone).first()
            if not mem:
                # formatted_number = phonenumbers.format_number(phonenumbers.parse(phone), phonenumbers.PhoneNumberFormat.INTERNATIONAL)
                # raise forms.ValidationError(_("A user with phone: {} not found").format(formatted_number))
                raise forms.ValidationError(_("A user with phone: {} not found").format(phone))
        else:
            raise forms.ValidationError(_("Invalid phone number: {}").format(phone))
        return mem

    @classmethod
    def get_member_by_email(cls, email, exclude_me=None):
        email = cls.normalize_email(email)
        if email:
            mem = Member.objects.filter_by_email(email).first()
            if not mem:
                raise forms.ValidationError(_("A user with email: {} not found").format(email))
        else:
            raise forms.ValidationError(_("Invalid email address: {}").format(email))
        return mem

    @classmethod
    def get_member_by_userid(cls, userid, exclude_me=None):
        mem = Member.objects.filter_by_userid(userid).first()
        if not mem:
            raise forms.ValidationError(_("A user with UserID: {} not found").format(userid))
        return mem

    @classmethod
    def save_user(cls, request, user, userdata):
        user.displayname = userdata.get('displayname', '')
        user.email = userdata.get('email')
        user.phone = userdata.get('phone')
        user.riscos = userdata.get('riscos','')
        user.first_name = userdata.get('first_name','')
        user.last_name = userdata.get('last_name','')
        user.userid = userdata.get('userid','')
        passw = userdata.get('password')
        if passw:
            user.set_password(passw)
        user.is_active = True

        user.push_channel = Member.objects.generate_push_channel_name()
        user.save()

        member_signed_up.send(Member, request=request, member=user)
        return user
