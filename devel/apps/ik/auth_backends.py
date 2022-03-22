import logging
from django.contrib.auth.backends import ModelBackend

from ik.models import Member
from ik.account_adapter import IKAccountAdapter
logger = logging.getLogger("cc")


class IKAuthenticationBackend(ModelBackend):

    def authenticate(self, request, email=None, username=None, password=None):
        user = None

        if username and not email:
            email = username

        if email:
            email = IKAccountAdapter.normalize_email(email)
            if email:
                user = Member.objects.filter_by_email(email).first()

        if user and user.is_active and user.check_password(password):
            return user

        logger.info(u"Auth failed email=[{}] pw=[{}]".format(email, password), extra={'debugdb':True})
        return None

    def get_user(self, user_id):
        try:
            return Member.objects.get(pk=user_id)
        except Member.DoesNotExist:
            return None

    def get_user_permissions(self, user):
        return set()