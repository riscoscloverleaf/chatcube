import logging
from datetime import timedelta
from django.core.management.base import BaseCommand
from django.utils.timezone import now

logger = logging.getLogger("cc")

class Command(BaseCommand):
    help = "Cleanup old authorized devices"

    def handle(self, *args, **options):
        from ik.api.models import AuthorizedDevice
        dt = now() - timedelta(days=180)
        deleted_devs = AuthorizedDevice.objects.filter(was_online__lt=dt).exclude(platform=AuthorizedDevice.PLATFORM_SYSTEM).delete()
        self.stdout.write(self.style.SUCCESS('Removed %s old authorized devices' % deleted_devs['api.AuthorizedDevice']))

