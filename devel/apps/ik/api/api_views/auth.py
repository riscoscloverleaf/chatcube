import logging
from django.contrib.auth.signals import user_logged_in
from django.views.decorators.debug import sensitive_post_parameters
from django.utils.translation import ugettext as _
from django.utils.decorators import method_decorator
from rest_framework import status
from rest_framework.exceptions import ValidationError
from rest_framework.permissions import AllowAny
from rest_framework.response import Response
from rest_framework.views import APIView
from django.template import loader
from ik.account_adapter import IKAccountAdapter
from ik.models.members import MemberLoginCode, Member
from ..permissions import IsAuthenticated
from ..serializers.auth import LoginSerializer, SignupSerializer, ChangePasswordSerializer, SetPasswordSerializer, VerifyCodeSerializer
from ..serializers.profile import MyProfileSerializer
from ..utils import make_authorized_device_and_get_new_token
from ..models import AuthorizedDevice
from ik.utils.sms import send_sms
from ik.utils.email import send_email_template

logger = logging.getLogger("cc")

class IKLoginMixin(object):
    def login_user(self, request, data):
        user = data['user']
        dev = make_authorized_device_and_get_new_token(user, data, request)

        # noinspection PyUnresolvedReferences
        user_logged_in.send(sender=user.__class__, request=request, user=user)

        user.online.set_online()

        return {
            'user': MyProfileSerializer(user).data,
            'token': dev.token
        }

class LoginMemberView(IKLoginMixin, APIView):
    permission_classes = (AllowAny,)

    @method_decorator(sensitive_post_parameters('password'))
    def dispatch(self, *args, **kwargs):
        return super(LoginMemberView, self).dispatch(*args, **kwargs)

    def post(self, request, *args, **kwargs):
        if request.user and hasattr(request.user, 'auth'):
            request.user.auth.delete()

        login_serializer = LoginSerializer(data=request.data)
        login_serializer.is_valid(raise_exception=True)

        return Response(self.login_user(request, login_serializer.validated_data), status=status.HTTP_200_OK)


class SignupMemberView(APIView):
    permission_classes = (AllowAny,)

    def post(self, request, *args, **kwargs):
        logger.info("SignupMemberView data={}".format(request.data))
        signup_serializer = SignupSerializer(data=request.data, context={'request': self.request})
        signup_serializer.is_valid(raise_exception=True)
        user = signup_serializer.save()

        dev = make_authorized_device_and_get_new_token(user, signup_serializer.validated_data, request)

        data = {
            'user': MyProfileSerializer(user).data,
            'token': dev.token
        }

        return Response(data, status=status.HTTP_200_OK)


class LogoutView(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        auth = request.auth
        # if auth.platform == 'browser':
        #     destroy_php_session(auth.token)
        auth.delete()
        return Response('ok', status=status.HTTP_200_OK)


class ChangePasswordView(APIView):
    permission_classes = (IsAuthenticated,)

    @method_decorator(sensitive_post_parameters('old_password','password'))
    def dispatch(self, *args, **kwargs):
        return super(ChangePasswordView, self).dispatch(*args, **kwargs)

    def post(self, request, *args, **kwargs):
        me = request.user

        serializer = ChangePasswordSerializer(me, data=request.data)
        serializer.is_valid(raise_exception=True)

        validated_data = serializer.validated_data

        me.set_password(validated_data['password'])
        me.push_channel = Member.objects.generate_push_channel_name()
        me.save()

        request.auth.regenerate_token()

        return Response({'token': request.auth.token, 'push_channel': me.push_channel}, status=status.HTTP_200_OK)


class ResetPasswordView(IKLoginMixin, APIView):
    permission_classes = (AllowAny,)

    @method_decorator(sensitive_post_parameters('password1', 'password2'))
    def dispatch(self, *args, **kwargs):
        return super(ResetPasswordView, self).dispatch(*args, **kwargs)

    def post(self, request, *args, **kwargs):
        serializer = SetPasswordSerializer(data=request.data)
        serializer.is_valid(raise_exception=True)
        validated_data = serializer.validated_data

        me = validated_data['user']
        me.set_password(validated_data['password1'])
        me.push_channel = Member.objects.generate_push_channel_name()
        me.save()

        code_obj = validated_data['code_obj']
        code_obj.delete()

        AuthorizedDevice.objects.filter(member=me).delete()
        logger.info("Password reset. code %s, member: %s", code_obj.code, me)
        return Response({"push_channel": me.push_channel}, status=status.HTTP_200_OK)


class SendCodeView(APIView):
    permission_classes = (AllowAny,)

    def dispatch(self, *args, **kwargs):
        return super(SendCodeView, self).dispatch(*args, **kwargs)

    def post(self, request, *args, **kwargs):
        email = request.POST.get('email')
        phone = request.POST.get('phone')
        country_code = request.POST.get('country_code')
        if not phone and not email:
            raise ValidationError(_("Either 'email' or 'phone' must be set"))

        if phone:
            mem = IKAccountAdapter.get_member_by_phone(phone, country_code)
            code_obj = MemberLoginCode.objects.make_code(mem)
            ctx = {'code': code_obj.code}
            text = loader.get_template("ik/sms/reset_password.txt").render(ctx)
            result = send_sms(mem.phone, text)
            if not result['error']:
                logger.info("Send SMS with verification code. code: %s, member: %s, phone: %s", code_obj, mem, mem.phone)
            else:
                logger.error("Failed to send SMS with verification code. code: %s, member: %s, phone: %s, error:%s", code_obj.code, mem, mem.phone, result['error'])
                raise ValidationError(_("SMS is failed to deliver to phone number {0}. Error: {1}").format(mem.phone, result['error']))

            resp = {'phone': mem.phone}
        else:
            mem = IKAccountAdapter.get_member_by_email(email)
            code_obj = MemberLoginCode.objects.make_code(mem)
            ctx = {'code': code_obj.code}
            send_email_template("reset_password.html", ctx, mem.email)
            logger.info("Send verification code. code: %s, member: %s, email: %s", code_obj.code, mem, mem.email)
            resp = {'email': mem.email}

        return Response(resp, status=status.HTTP_200_OK)


class VerifyCodeView(APIView):
    permission_classes = (AllowAny,)

    def post(self, request, *args, **kwargs):
        verify_serializer = VerifyCodeSerializer(data=request.data)
        verify_serializer.is_valid(raise_exception=True)

        return Response("ok", status=status.HTTP_200_OK)
