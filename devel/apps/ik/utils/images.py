import logging
import os

from PIL import Image
from django.conf import settings
from easy_thumbnails.files import get_thumbnailer

from ik.utils.site import build_absolute_uri

logger = logging.getLogger("cc")

def image_as_dict(img, default_img):
    try:
        photo_file = os.path.join(settings.MEDIA_ROOT, img)
        image = Image.open(photo_file)

        thumb = get_thumbnailer(img).get_thumbnail(settings.ATTACHMENT_THUMBNAIL_CONF)
        thumb_url = build_absolute_uri(thumb.url)
        thumb_width = thumb.width
        thumb_height = thumb.height

        return {
            'url': build_absolute_uri(os.path.join(settings.MEDIA_URL, img)),
            'size': os.path.getsize(photo_file),
            'thumb_url': thumb_url,
            'thumb_width': thumb_width,
            'thumb_height': thumb_height,
            'width': image.width,
            'height': image.height
        }
    except Exception as ex:
        if img != default_img:
            logger.exception("Error in image: {}, {}".format(img, ex))
            return image_as_dict(default_img, default_img)

        logger.exception("Error in default image: {}, {}".format(img, ex))
        return None
