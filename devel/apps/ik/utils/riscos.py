import os
import logging
import re
from django.conf import settings

logger = logging.getLogger('ik')

riscos_file_types = {
    0xFF0: "tiff",
    0xFD9: "exe",
    0xFD8: "com",
    0xFC2: "aiff",
    0xFB2: "avi",
    0xFB1: "wav",
    0xFAF: "html",
    0xF98: "psd",
    0xF89: "gz",
    0xF83: "mng",
    0xF81: "js",
    0xF80: "xml",
    0xF79: "css",
    0xF78: "jng",
    0xDFE: "csv",
    0xDF6: "iso",
    0xDEA: "dxf",
    0xDDC: "zip",
    0xDB0: "wk1",
    0xD6D: "rle",
    0xCFF: "cff",
    0xCE5: "tex",
    0xCE4: "dvi",
    0xCB6: "mod",
    0xC85: "jpg",
    0xC46: "tar",
    0xC32: "rtf",
    0xBF8: "mpg",
    0xBA6: "xls",
    0xB61: "xbm",
    0xB60: "png",
    0xB2F: "wmf",
    0xADF: "pdf",
    0xAE4: "jar",
    0xABF: "cab",
    0xAAD: "svg",
    0xAA7: "m3u",
    0xA91: "zip",
    0xA8F: "ac3",
    0xA8D: "vob",
    0xA7F: "xlsx",
    0xA7E: "docx",
    0x808: "eps",
    0x77F: "ttf",
    0x69E: "ppm",
    0x69C: "bmp",
    0x697: "pcx",
    0x695: "gif",
    0x1CF: "flac",
    0x1AD: "mp3",
    0x1A8: "ogg",
    0x071: "avi"
}

def riscos2common_name(ro_name, ro_file_type):
    try:
        ext = riscos_file_types.get(int(ro_file_type))
        name = ro_name.split(".")[-1].split(",")[0]
        result = "{}.{}".format(name, ext)
    except:
        result = ro_name.split(".")[-1]
    return result

_image_type_riscos = (
    0xFF0, 0xC85, 0xB60, 0x695
)

def riscos_is_image_type(ro_file_type):
    return ro_file_type in _image_type_riscos

_mime_map = None
def mime_to_riscos_type(mime_type):
    global _mime_map
    if not _mime_map:
        with open(os.path.join(settings.PROJECT_DIR, "var", "global", "MimeMap"), "rt") as f:
            _mime_map = {}
            for line in f:
                line = line.strip()
                if line and line[0] != '#':
                    parts = re.split(r'\s+', line)
                    try:
                        _mime_map[parts[0]] = int(parts[2], 16)
                    except:
                        pass

    ro_type = _mime_map.get(mime_type)
    if ro_type:
        return ro_type

    if mime_type[:5] == "text/":
        return 0xfff

    return 0xffd

class ChatCubeVersion(object):
    _messages_tm = 0
    _version = None

    @classmethod
    def get_version_int_from_str(cls, ver_str):
        if not ver_str:
            return 0
        i = 0
        ver = 0
        ver_arr = ver_str.split(".")
        ver_arr.reverse()
        for n in ver_arr:
            ver += int(n) * (10000 ** i)
            i += 1
        return ver

    @classmethod
    def get_version(cls):
        messages_file = os.path.join(os.path.dirname(settings.PROJECT_DIR), "!ChatCube", "Messages")
        mtm = os.path.getmtime(messages_file)
        ver = None
        if cls._messages_tm != mtm:
            with open(messages_file) as f:
                for line in f:
                    parts = line.strip().split(":")
                    if len(parts) == 2 and parts[0] == 'Version':
                        ver = parts[1]
                        break
            if not ver:
                logging.error("App version not found in {}".format(messages_file))
            else:
                cls._version = ver
            cls._messages_tm = mtm
        return cls._version

