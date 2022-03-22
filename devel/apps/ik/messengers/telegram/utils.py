import hashlib
import os
from django.conf import settings

def telegram_files_directory(phone):
    hasher = hashlib.md5()
    hasher.update(("FILES!" + str(phone)).encode('utf-8'))
    directory_name = hasher.hexdigest()

    return os.path.join(settings.PROJECT_DIR, "var", "global", "messengers", "telegram",
                         directory_name[:2], directory_name[2:])
