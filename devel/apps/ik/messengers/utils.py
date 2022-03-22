import logging
from ik.constants import MESSENGER_TG, MESSENGER_CHATCUBE, MESSENGER_FB
from rest_framework.exceptions import NotFound

logger = logging.getLogger("cc")

def split_messeger_id(mid):
    messenger = mid[0]
    id = int(mid[1:])

    if messenger != MESSENGER_CHATCUBE and messenger != MESSENGER_TG:
        msg = "Wrong messenger '{}' in {}".format(messenger, mid)
        logger.error(msg)
        raise NotFound(msg)

    return messenger, id
