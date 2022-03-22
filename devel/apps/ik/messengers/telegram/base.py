import uuid
import json
import logging
import time
import os
from pathlib import PosixPath

from django_redis import get_redis_connection
from django.conf import settings

logger = logging.getLogger('ik')

class IKTelegramBase(object):
    TG_REDIS_UPDATES_QUEUE = 'tg:upd'
    TG_REDIS_REQ_QUEUE = 'tg:req'
    TG_REDIS_RESP_QUEUE_PREFIX = 'tg:rsp:'
    TG_REDIS_FILE_DOWNLOAD_PREFIX = 'tg:file'

    in_progress_auth_states = ('authorizationStateWaitEncryptionKey')
    def __init__(self):
        self.redis_conn = get_redis_connection('messengers')

    def telegram_enqueue_update(self, account_id, data):
        self.redis_conn.rpush(self.TG_REDIS_UPDATES_QUEUE, json.dumps((account_id, time.time(), data), ensure_ascii=False))

    def telegram_put_response(self, account_id, req_id, data):
        resp_queue = "{}:{}:{}".format(self.TG_REDIS_RESP_QUEUE_PREFIX, account_id, req_id)
        self.redis_conn.rpush(resp_queue, json.dumps(data, ensure_ascii=False))
        self.redis_conn.expire(resp_queue, 3) # expire response in case if client was timed out already

    def telegram_get_response(self, account_id, req_id, timeout):
        res = self.redis_conn.blpop("{}:{}:{}".format(self.TG_REDIS_RESP_QUEUE_PREFIX, account_id, req_id), timeout)
        if not res:
            return None
        response = json.loads(res[1])
        logger.debug("Telegram get response. account={} reqid={} response={}".format(account_id, req_id, res[1][:1000]))
        return response

    def telegram_send_request(self, account_id, name, req_id=None, tg_timeout=30, **kwargs):
        # will expire at this time.
        # It should not sent to telegram after this time as client does not interested in result
        expire_at = time.time()+tg_timeout
        req = (
            account_id,
            expire_at,
            req_id,
            name,
            kwargs
        )
        logger.debug("Telegram request sent. account={} req_id={} method={} params={}".format(account_id, req_id, name, kwargs))
        self.redis_conn.rpush(self.TG_REDIS_REQ_QUEUE, json.dumps(req, ensure_ascii=False))

    def telegram_call_method(self, account_id, name, req_id=None, tg_timeout=30, **kwargs):
        if not req_id:
            req_id = uuid.uuid4().hex

        self.telegram_send_request(account_id, name, req_id, tg_timeout, **kwargs)
        response = self.telegram_get_response(account_id, req_id, tg_timeout)
        if response is None:
            raise TimeoutError("Telegram request timeout, account={} method={} params={}".format(account_id, name, kwargs))
        return response


    def telegram_get_local_file(self, file_data, name_generator):
        if file_data['local']['path'] and file_data['local']['is_downloading_completed'] and os.path.exists(file_data['local']['path']):
            local = name_generator(file_data['local']['path'])
            local_path = os.path.join(settings.MEDIA_ROOT, local)
            if not os.path.exists(local_path):
                dir = os.path.dirname(local_path)
                if not os.path.isdir(dir):
                    PosixPath(dir).mkdir(parents=True, exist_ok=True)
                try:
                    os.unlink(local_path)
                except:
                    pass
                try:
                    os.symlink(file_data['local']['path'], local_path)
                except FileExistsError:
                    pass
            return local
        return None