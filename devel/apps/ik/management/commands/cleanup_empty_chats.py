import logging
from django.core.management.base import BaseCommand, CommandError
from django.db.models import Count

from ik.models import Member

logger = logging.getLogger("cc")

class Command(BaseCommand):
    help = "Cleanup empty chats (chats with zero members or private chats with 1 member only)"

    def handle(self, *args, **options):
        from ik.models.chat import Chat, ChatMember
        Chat.objects.all().annotate(chatmems=Count('chatmembers')).filter(chatmems=0).delete()
        Chat.objects.filter(is_group=False).annotate(chatmems=Count('chatmembers')).filter(chatmems=1).delete()

