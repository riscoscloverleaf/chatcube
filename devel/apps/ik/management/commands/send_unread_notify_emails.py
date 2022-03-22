import os
import logging
import time
import datetime
from easy_thumbnails.models import Source
from django.core.files.storage import default_storage
from django.core.management.base import BaseCommand, CommandError
from django.db.models import F,Max,OuterRef,Subquery
from django.utils.timezone import now
from ik.models.members import Member
from ik.models.chat import ChatMember, Message, Chat
from ik.utils.email import can_send_email_to, send_email_template
from ik.constants import *

logger = logging.getLogger("cc")

class Command(BaseCommand):
    help = "Send unread message notify email to members"

    def send_to_member(self, member, unread_chats):
        if can_send_email_to(member):
            self.stdout.write(self.style.SUCCESS('Send to member %s' % member))
            total_unread = 0
            for unr in unread_chats:
                total_unread += unr["unread_count"]
            ctx = {
                "member": member,
                "unread_chats": unread_chats,
                "total_unread": total_unread
            }
            send_email_template("unread_messages.html", ctx, member.email, "cc_unread",
                                from_email="unread@chatcube.org", member=member)
            member.settings.set_value("unread_sent", int(time.time()))

    def add_arguments(self, parser):
        parser.add_argument('--testmember', type=str, help="EMAIL of member")

    def handle(self, *args, **options):
        testmember = options['testmember']
        if testmember:
            member = Member.objects.get(email=testmember)
            unread_msgs = [{
                'unread_count': 12,
                "is_group": 1,
                "chat_title": "Test Group Tile",
                "last_message": Message.objects.filter(type=MESSAGE_TYPE_TEXT).first()
                },
                {
                    'unread_count': 12,
                    "is_group": 0,
                    "chat_title": "Test Chat Tile",
                    "last_message": Message.objects.filter(type=MESSAGE_TYPE_TEXT).first()
                }]
            self.send_to_member(member, unread_msgs)
            return

        latest_msg = Message.objects.filter(chat=OuterRef('chat')).order_by("-id")
        chatmembers = ChatMember.objects.filter(last_seen_message_id__lt=Subquery(latest_msg.values('id')[:1]))\
            .select_related('member', 'chat', 'last_seen_message')\
            .order_by('member')
        prev_cm = None
        unread_chats = []
        is_old_unread_messages_exists = False
        unread_age = -1
        sent_already = False
        for cm in chatmembers:
            if prev_cm != cm.member:
                if prev_cm:
                    if unread_age > 0:
                        if is_old_unread_messages_exists and not sent_already and len(unread_chats) > 0:
                            self.send_to_member(prev_cm, unread_chats)
                        else:
                            self.stdout.write('Member %s, skip as no old unseen messages' % prev_cm)
                    else:
                        self.stdout.write('Member %s, skip as set unread age to -1' % prev_cm)
                notification_settings = cm.member.notification_settings()
                unread_age = notification_settings.get("unread_age", 48)
                sent_date = cm.member.settings.get_value("unread_sent", 0)
                sent_already = (((sent_date + unread_age * 3600) - 3600) > int(time.time()))
                is_old_unread_messages_exists = False
                unread_chats = []
                prev_cm = cm.member

            if not is_old_unread_messages_exists and unread_age > 0 and not sent_already:
                unread_date = now() - datetime.timedelta(seconds=unread_age*3600)
                is_old_unread_messages_exists = Message.objects.filter(chat=cm.chat, sendtime__lt=unread_date, id__gt=cm.last_seen_message.id)\
                    .exclude(author=cm.member).exists()

            if not sent_already and unread_age > 0:
                party_chatmembers = cm.chat.get_party_chatmembers(cm.member, limit=4)
                cnt = cm.chat.get_unread_count(cm)
                if cnt > 0:
                    unread_chats.append({"unread_count": cnt,
                           "is_group": cm.chat.is_group,
                           "chat_title": cm.chat.get_title(party_chatmembers),
                           "last_message": cm.chat.get_last_message(cm.member)
                           })

        if unread_chats:
            if unread_age > 0:
                if is_old_unread_messages_exists:
                    self.send_to_member(cm.member, unread_chats)
                else:
                    self.stdout.write('Member %s, skip as no old unseen messages' % cm.member)
            else:
                self.stdout.write('Member %s, skip as set unread age to -1' % cm.member)

