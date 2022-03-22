import logging
import os
from ik.messengers.telegram.models import TelegramAccount
from rest_framework.response import Response
from rest_framework.views import APIView
from rest_framework.exceptions import ValidationError
from django.db.models import ObjectDoesNotExist
from django.http.response import HttpResponseRedirect
from django.conf import settings
from ik.api.permissions import IsAuthenticated
from ik.api.serializers.telegram import TelegramRegisterPhoneSerializer, TelegramRegisterMemberSerializer
from ik.messengers.telegram.client import IKTelegramClient, name_generators
from ik.utils.site import build_absolute_uri

logger = logging.getLogger("cc")

class TelegramRegisterOrChangePhoneView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        serializer = TelegramRegisterPhoneSerializer(data=request.POST)
        serializer.is_valid(raise_exception=True)
        data = serializer.validated_data
        exists = TelegramAccount.objects.filter(member=request.user).exists()
        if exists:
            IKTelegramClient.change_account_phone(request.user, data['phone'])
        else:
            IKTelegramClient.register_account(request.user, data['phone'])
        return Response({"result": "ok"})


class TelegramAuthCode(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        client = IKTelegramClient.get_client(request.user)
        if request.POST.get('resend'):
            client.resend_code()
        else:
            client.check_code(request.POST['code'].strip())
        return Response({"result": "ok"})


class TelegramAuthPassword(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        client = IKTelegramClient.get_client(request.user)
        client.check_password(request.POST['password'].strip())
        return Response({"result": "ok"})


class TelegramRegisterMemberView(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        serializer = TelegramRegisterMemberSerializer(data=request.POST)
        serializer.is_valid(raise_exception=True)
        data = serializer.validated_data

        client = IKTelegramClient.get_client(request.user)
        client.register_user(data['first_name'], data.get('last_name',''))
        return Response({"result": "ok"})


class TelegramAcceptTOS(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        tos_id = request.POST.get("tos_id")
        if not tos_id:
            raise ValidationError("tos_id must not me empty")

        client = IKTelegramClient.get_client(request.user)
        client.accept_tos(tos_id)
        return Response({"result": "ok"})


class TelegramDeleteAccount(APIView):
    permission_classes = (IsAuthenticated,)

    def post(self, request, *args, **kwargs):
        client = IKTelegramClient.get_client(request.user)
        client.delete_account(request.user, request.POST.get("reason",""))
        return Response({"result": "ok"})

class TelegramUnregisterAccount(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        try:
            client = IKTelegramClient.get_client(request.user)
            client.stop()
            request.user.telegram_account.delete()
        except ObjectDoesNotExist:
            raise ValidationError("Telegram account not exists")
        return Response({"result": "ok"})

    def post(self, request, *args, **kwargs):
        return self.get(request, args, kwargs)


class TelegramGetMe(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        client = IKTelegramClient.get_client(request.user)
        return Response({"result": client.get_me()})


class TelegramGetContacts(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        client = IKTelegramClient.get_client(request.user)
        return Response(client.get_contacts())


class TelegramSearchPublicChats(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        client = IKTelegramClient.get_client(request.user)
        return Response(client.search_public_chats(request.GET.get("query","")))


class TelegramDownloadFile(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, file_kind=None, file_id=None, **kwargs):
        client = IKTelegramClient.get_client(request.user)
        name_generator = name_generators.get(file_kind, name_generators['attachments'])
        file_name = client.download_file_by_id(file_id, name_generator, None)
        if not file_name:
            raise ValidationError("Error download file id: %s" % (file_id,))


        return HttpResponseRedirect(build_absolute_uri(os.path.join(settings.MEDIA_URL, file_name)))
