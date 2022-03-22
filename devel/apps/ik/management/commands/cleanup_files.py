import os
import logging
import time
from easy_thumbnails.models import Source
from django.core.files.storage import default_storage
from django.core.management.base import BaseCommand, CommandError

logger = logging.getLogger("cc")

class Command(BaseCommand):
    help = "Cleanup (delete temp uploads and missing thumbails)"

    def handle(self, *args, **options):
        keep_time = int(time.time()) - 86400
        temp_uploads = default_storage.path("temp_uploads")
        for root, dirs, files in os.walk(temp_uploads, topdown=False):
            for name in files:
                fullname = os.path.join(root, name)
                if os.stat(fullname).st_mtime < keep_time:
                    os.unlink(fullname)
                    self.stdout.write(self.style.SUCCESS('Removed old uploaded file: %s' % fullname))
            for name in dirs:
                fullname = os.path.join(root, name)
                try:
                    os.rmdir(fullname)
                except:
                    pass

        telegram_files = default_storage.path("telegram")
        for root, dirs, files in os.walk(telegram_files, topdown=False):
            for name in files:
                fullname = os.path.join(root, name)
                if os.path.islink(fullname) and not os.path.exists(fullname):
                    os.unlink(fullname)
                    self.stdout.write(self.style.SUCCESS('Removed broken synlink: %s' % fullname))

        for thumb in Source.objects.all():
            if not default_storage.exists(thumb.name):
                thumb.delete()
                self.stdout.write(self.style.SUCCESS('Removed non-exists thumbnail source: %s' % thumb.name))


