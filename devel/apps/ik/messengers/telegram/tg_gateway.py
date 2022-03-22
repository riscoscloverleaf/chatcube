import signal
import json
import os
import time
import logging
import threading
import hashlib
from django.conf import settings
from telegram import VERSION
from telegram.tdjson import TDJson

from .base import IKTelegramBase
from .utils import telegram_files_directory

logger = logging.getLogger('ik')


enabledUpdates = set()

authMethods = ('checkAuthenticationCode', 'resendAuthenticationCode', 'checkAuthenticationPassword', 'registerUser')

class IKTelegramGateway(IKTelegramBase):
    def __init__(
        self,
        account_id,
        phone,
        files_directory,
        use_test_dc = False,
        system_language_code='en',
    ) -> None:
        """
        Args:
            api_id - ID of your app (https://my.telegram.org/apps/)
            api_hash - api_hash of your app (https://my.telegram.org/apps/)
            phone - your phone number
            library_path - you can change path to the compiled libtdjson library
            files_directory - directory for the tdlib's files (database, images, etc.)
            use_test_dc - use test datacenter
        """
        if not phone:
            raise ValueError('You must provide phone to start gateway')

        super(IKTelegramGateway, self).__init__()

        self.account_id = account_id
        self.phone = phone
        self.use_test_dc = use_test_dc
        self.system_language_code = system_language_code
        self.files_directory = files_directory

        self.fully_started = False
        self._auth_state = None
        self._client_must_wait = True
        self._is_enabled = False
        self._sent_requests = set()
        self._auth_state_data = None
        self._tdjson = TDJson(library_path=None, verbosity=2)
        self._run()

    def __del__(self):
        self.stop()

    def stop(self):
        """Stops the client"""
        if not self._is_enabled:
            return

        self._is_enabled = False
        logger.info('[{}] gateway stopping...'.format(self.account_id))
        self._td_listener.join(15)

        if hasattr(self, '_tdjson'):
            self._tdjson.stop()

        logger.info('[{}] gateway stopped'.format(self.account_id))


    def _run(self):
        self._is_enabled = True

        self._td_listener = threading.Thread(target=self._listen_to_td)
        self._td_listener.daemon = True
        self._td_listener.start()

        self._send_data({'@type': 'getAuthorizationState'})

    def _listen_to_td(self):
        logger.info('[{}] td_listener started'.format(self.account_id))

        while self._is_enabled:
            update = self._tdjson.receive()

            if update:
                self._process_async_update(update)

        logger.info('[{}] td_listener stopped'.format(self.account_id))

    def _process_async_update(self, update):
        #logger.debug("Get update: {}".format(update))
        if '@extra' in update:
            request_id = update['@extra'].get('request_id')
        else:
            request_id = None

        # special case process for initial login squence
        if update['@type'] == 'updateAuthorizationState':
            self._process_authorization_update(update)
            return

        # update is not response on specific call but update from Telegram, put it to updates queue
        if not request_id:
            if update['@type'] in enabledUpdates:
                self.telegram_enqueue_update(self.account_id, update)
            return

        if request_id not in self._sent_requests:
            logger.debug('[%s] corresponding request was not sent with request_id=%s', self.account_id, request_id)
            return

        self._sent_requests.remove(request_id)

        if request_id == 'getChatsInitial':
            self._chatsLoaded()
        else:
            if update['@type'] == 'error' and update['code'] == 401:
                update['details_code'] = self._auth_state
            self.telegram_put_response(self.account_id, request_id, update)


    def _send_data(self, data, request_id=None):
        """
        Sends data to tdlib.
        """
        if request_id:
            if '@extra' not in data:
                data['@extra'] = {}
            data['@extra']['request_id'] = request_id
            self._sent_requests.add(request_id)

        self._tdjson.send(data)

    def _signal_handler(self, signum, frame):
        self._is_enabled = False


    def _process_authorization_update(self, update):
        self._auth_state = update['authorization_state']['@type']

        if self._auth_state == 'authorizationStateWaitTdlibParameters':
            logger.info(
                '[%s] Setting tdlib initial params: files_dir=%s, test_dc=%s',
                self.account_id,
                self.files_directory,
                self.use_test_dc,
            )
            self._client_must_wait = True
            data = {
                '@type': 'setTdlibParameters',
                'parameters': {
                    'use_test_dc': self.use_test_dc,
                    'api_id': settings.TELEGRAM_API_ID,
                    'api_hash': settings.TELEGRAM_API_HASH,
                    'device_model': 'python-telegram',
                    'system_version': 'unknown',
                    'application_version': VERSION,
                    'system_language_code': self.system_language_code,
                    'database_directory': os.path.join(self.files_directory, 'database'),
                    'use_message_database': True,
                    'files_directory': os.path.join(self.files_directory, 'files'),
                },
            }

            self._send_data(data)

        elif self._auth_state == 'authorizationStateWaitEncryptionKey':
            logger.info('[{}] sending encryption key'.format(self.account_id))
            self._client_must_wait = True
            hash = hashlib.md5()
            hash.update((self.phone+settings.TELEGRAM_API_HASH+"!DB").encode("utf-8"))
            data = {
                '@type': 'checkDatabaseEncryptionKey',
                'encryption_key': hash.hexdigest()[:12]
            }

            self._send_data(data)

        elif self._auth_state == 'authorizationStateWaitPhoneNumber':
            logger.info('[{}] sending phone number'.format(self.account_id))
            self._client_must_wait = True
            data = {
                '@type': 'setAuthenticationPhoneNumber',
                'phone_number': self.phone,
                'allow_flash_call': False,
                'is_current_phone_number': True,
            }
            self._send_data(data)

        elif self._auth_state == 'authorizationStateWaitCode':
            self._client_must_wait = False
            self._auth_state_data = update
            self.telegram_enqueue_update(self.account_id, update)

        elif self._auth_state == 'authorizationStateWaitPassword':
            self._client_must_wait = False
            self._auth_state_data = update
            self.telegram_enqueue_update(self.account_id, update)

        elif self._auth_state == 'authorizationStateWaitRegistration':
            self._client_must_wait = False
            self._auth_state_data = update
            self.telegram_enqueue_update(self.account_id, update)

        elif self._auth_state == 'authorizationStateReady':
            # self._client_must_wait = True
            # self._auth_state_data = None
            # logger.info('[{}] completing auth process.'.format(self.account_id))
            #self._send_data({'@type': 'getChats', 'offset_order': 9223372036854775807, 'limit': 100}, 'getChatsInitial')
            self._client_must_wait = False
            self.fully_started = True
            self.telegram_enqueue_update(self.account_id, "INSTANCE_STARTED")
            logger.info('[{}] gateway fully started.'.format(self.account_id))

    def _chatsLoaded(self):
        self._client_must_wait = False
        self.fully_started = True
        self.telegram_enqueue_update(self.account_id, "INSTANCE_STARTED")
        logger.info('[{}] gateway fully started.'.format(self.account_id))

    def get_auth_state(self):
        return self._auth_state

    def call_method(self, method_name, req_id=None, params = None):
        """
        Use this method to call any other method of the tdlib

        Args:
            method_name: Name of the method
            params: parameters
        """
        if self._client_must_wait and req_id:
            self.telegram_put_response(self.account_id, req_id, {'@type':'error',
                                                                 'code': 0,
                                                                 'message': 'Wait to start',
                                                                 'details_code': 'must_wait'})
            return

        if self._auth_state_data is not None and method_name not in authMethods:
            self.telegram_enqueue_update(self.account_id, self._auth_state_data)
            self.telegram_put_response(self.account_id, req_id, {'@type':'error',
                                                                 'code': 401,
                                                                 'message': 'Unautorized'})
            return


        data = {'@type': method_name}

        if params:
            data.update(params)

        self._send_data(data, req_id)


class TelegramGatewayProcess(IKTelegramBase):
    alive = False
    instances = {}

    def __init__(self):
        super().__init__()

        for sig in (signal.SIGINT, signal.SIGTERM, signal.SIGABRT):
            signal.signal(sig, self.stop_signal)

    def stop_signal(self, signum, frame):
        logger.info("Got signal %d, exiting...", signum)
        self.exit()

    def exit(self):
        self.alive = False
        for c in self.instances.keys():
            self.instances[c].stop()
        time.sleep(1)

    def start_instance(self, account_id, req_id, params):
        if account_id not in self.instances:
            files_dir = telegram_files_directory(params['phone'])
            self.instances[account_id] = IKTelegramGateway(
                account_id,
                params['phone'],
                files_directory=files_dir,
                use_test_dc=True if os.getenv('TG_USE_TEST_DC') == "1" else False,
                system_language_code=params.get('lang', 'en'))
        if req_id:
            self.telegram_put_response(account_id, req_id, {'@type': 'ok'})

    def stop_instance(self, account_id, req_id):
        if account_id in self.instances:
            self.instances[account_id].stop()
            del self.instances[account_id]
        if req_id:
            self.telegram_put_response(account_id, req_id, {'@type': 'ok'})
        self.telegram_enqueue_update(account_id, "INSTANCE_STOPPED")

    def get_running_gateways(self, account_id, req_id):
        result = [inst for inst in self.instances.keys() if self.instances[inst].fully_started]
        self.telegram_put_response(account_id, req_id, {'@type': 'ok', 'result': result})

    def set_interested_updates(self, updates):
        enabledUpdates.clear()
        enabledUpdates.update(updates)

    def loop(self):
        self.alive = True

        self.telegram_enqueue_update(0, "GATEWAY_STARTED")

        while self.alive:
            req = self.redis_conn.blpop(self.TG_REDIS_REQ_QUEUE, 3)
            if req:
                queue, req_str = req
                if req_str == 'exit':
                    self.exit()
                    continue
                try:
                    account_id, expired_at, req_id, method_name, params = json.loads(req_str)
                except Exception as ex:
                    logger.exception("Invalid Telegram request [%s], error: %s", req_str, ex)
                    continue

                now = time.time()
                if expired_at < now:
                    logger.error("Telegram request {}:{} expired, timestamp:{} now:{}".format(account_id, method_name, expired_at, now))
                    continue

                logger.info("Telegram request {}:{}".format(account_id, method_name))

                if method_name == 'start':
                    self.start_instance(account_id, req_id, params)
                elif method_name == 'stop':
                    self.stop_instance(account_id, req_id)
                elif method_name == 'get_running_gateways':
                    self.get_running_gateways(account_id, req_id)
                elif method_name == 'set_interested_updates':
                    self.set_interested_updates(params['updates'])
                else:
                    if account_id not in self.instances:
                        self.telegram_put_response(account_id, req_id, {'@type':'error',
                                                                        'code': 0,
                                                                        'message': "Telegram Client not started",
                                                                        'details_code': 'not_run'})
                    else:
                        self.instances[account_id].call_method(method_name, req_id, params)
