import hashlib
import uuid
import os
from django.utils.deconstruct import deconstructible

@deconstructible
class UUIDPath:
    def __init__(self, prefix):
        self.prefix = prefix

    def __call__(self, instance, filename):
        uid = uuid.uuid4().hex
        name, ext = os.path.splitext(filename)
        return "{}/{}/{}/{}{}".format(self.prefix, uid[0:2], uid[2:4], uid[4:], ext)

@deconstructible
class MD5PathWithExt:
    def __init__(self, prefix):
        self.prefix = prefix

    def __call__(self, filename):
        hash = hashlib.md5()
        hash.update(filename.encode("utf-8"))
        uid = hash.hexdigest()
        name, ext = os.path.splitext(filename)
        return "{}/{}/{}/{}{}".format(self.prefix, uid[0:2], uid[2:4], uid[4:], ext)

@deconstructible
class MD5PathWithBasename:
    def __init__(self, prefix):
        self.prefix = prefix

    def __call__(self, filename):
        hash = hashlib.md5()
        hash.update(filename.encode("utf-8"))
        uid = hash.hexdigest()
        name = os.path.basename(filename)
        return "{}/{}/{}/{}/{}".format(self.prefix, uid[0:2], uid[2:4], uid[4:], name)

