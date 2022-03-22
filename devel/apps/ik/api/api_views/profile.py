import logging
import time
from django.contrib.auth.signals import user_logged_in
from django.utils.timezone import now
from django.http import Http404
from django.db import models
from django.db.models import Count
from rest_framework import status
from rest_framework.generics import RetrieveUpdateAPIView, RetrieveAPIView, ListAPIView
from rest_framework.views import APIView
from rest_framework.response import Response

from ik import constants
from ik.models import Member
from ik.signals import member_changed
from ik.messengers.utils import split_messeger_id
from ik.messengers.client import get_chat_client
from ik.messengers.telegram.client import IKTelegramClient
from ik.models.chat import ChatMember, Chat, AuthorizedDevice
from ..permissions import IsAuthenticated
from ..serializers.profile import MyProfileSerializer
from ..utils import get_app_timezone

logger = logging.getLogger("cc")

class MyProfileView(RetrieveUpdateAPIView):
    permission_classes = (IsAuthenticated,)
    serializer_class = MyProfileSerializer

    def get_serializer_context(self):
        ctx = super(MyProfileView, self).get_serializer_context()
        ctx['only'] = tuple(filter(lambda x: x != '', map(lambda x: x.strip(), self.request.GET.get('only','').split(","))))
        return ctx

    def retrieve(self, request, *args, **kwargs):
        instance = self.get_object()
        reqdata = self.request.GET
        initial = reqdata.get('initial')
        if initial:
            # logger.info(u"Member {} loads initial profile".format(instance))

            try:
                tgacc = instance.telegram_account
                if not tgacc.tg_user_id:
                    client = IKTelegramClient(tgacc)
                    client.get_me()
            except models.ObjectDoesNotExist:
                pass

            app_ver = reqdata.get('app_version')
            if app_ver:
                # tz = reqdata.get('timezone')
                # if tz:
                #     request.auth.timezone = get_app_timezone(tz, reqdata.get('utc_offset'))
                request.auth.app_version = app_ver
                request.auth.was_online = now()
                request.auth.save()

            instance.online.set_online()
            user_logged_in.send(Member, user=instance, request=request)

        serializer = self.get_serializer(instance)
        return Response(serializer.data)

    def post(self, request, *args, **kwargs):
        return self.partial_update(request, *args, **kwargs)

    def get_object(self):
        return self.request.user



class MyProfileDelete(APIView):
    permission_classes = (IsAuthenticated,)

    def do_delete(self, member):
        member.mark_as_deleted()
        member_changed.send(sender=Member, member=member)

        ChatMember.objects.filter(member=member, chat__is_group=False).delete()
        for c in Chat.objects.filter(is_group=False).annotate(chatmems=Count('chatmembers')).filter(chatmems__lte=1):
            if c.chatmems > 0:
                cm = ChatMember.objects.filter(chat=c).first()
                cm.chat.leave_chat(cm.member)

        AuthorizedDevice.objects.filter(member=member).delete()

    def post(self, request, *args, **kwargs):
        self.do_delete(self.request.user)
        return Response({'result': 'ok'}, status=status.HTTP_200_OK)



class ProfileView(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        messenger, uid = split_messeger_id(kwargs['user_id'])
        client = get_chat_client(messenger, request.user)
        return Response(client.get_user_as_dict(uid))


class ProfilesListView(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, *args, **kwargs):
        users = {}
        for user_id in request.GET.get('ids', '').split(","):
            messenger, uid = split_messeger_id(user_id)
            if messenger not in users:
                users[messenger] = []
            users[messenger].append(uid)

        result = []
        for messenger, uids in users.items():
            client = get_chat_client(messenger, request.user)
            result.extend(client.get_users_as_dict(uids, raise_not_found_exception=False))

        return Response(result)


class ProfileSearch(APIView):
    permission_classes = (IsAuthenticated,)

    def get(self, request, user_id=None):
        return Response("Not implemented")

