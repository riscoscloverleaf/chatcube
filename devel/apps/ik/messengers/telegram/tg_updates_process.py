import signal
import json
import logging
import time
from django.conf import settings
from django.db import connection, connections
from .models import TelegramAccount
from .base import IKTelegramBase
from .client import IKTelegramClient, TelegramUserConverter, TelegramMessageConverter, TELEGRAM2CHAT_ACTIONS
from ik.constants import *
from ik.events import Event
from ik.utils.site import build_absolute_uri

logger = logging.getLogger('ik')


class TelegramUpdatesProcess(IKTelegramBase):
    alive = False
    telegram_clients = {}
    telegram_gateways_ready = set()

    def __init__(self):
        super().__init__()

        for sig in (signal.SIGINT, signal.SIGTERM, signal.SIGABRT):
            signal.signal(sig, self.stop_signal)

    def get_running_gateways(self):
        response = self.telegram_call_method(0, 'get_running_gateways')
        return response['result']

    def set_interested_updates(self):
        updates = [k for k in dir(self) if k[:6] == 'update' and callable(getattr(self,k))]
        self.telegram_send_request(0, 'set_interested_updates', updates=updates)

    def get_telegram_client(self, account_id):
        if account_id not in self.telegram_clients:
            try:
                tg_account = TelegramAccount.objects.get(pk=account_id)
                self.telegram_clients[account_id] = IKTelegramClient(tg_account)
            except TelegramAccount.DoesNotExist:
                return None
        return self.telegram_clients[account_id]

    def stop_signal(self, signum, frame):
        logger.info("Got signal %d, exiting...", signum)
        self.alive = False

    # update handlers below must be named exactly as telegram update types

    def updateChatOrder(self, update, client):
        if update['order'] == '0':
            Event.send(client.account.member, Event.EV_CHAT_DELETED, {'chat_id': MESSENGER_TG+str(update['chat_id'])})
        else:
            chat_dict = client.get_chat(update['chat_id'])
            Event.send(client.account.member, Event.EV_CHAT_CREATED, chat_dict)

    # def updateNewChat(self, update, client):
    #     chat_dict = TelegramChatConverter.convert(update['chat'], client)
    #     Event.send(client.account.member, Event.EV_CHAT_CREATED, chat_dict)

    def updateUser(self, update, client):
        user_dict = TelegramUserConverter.convert(update['user'], client)
        Event.send(client.account.member, Event.EV_MEMBER_UPDATED, user_dict)

    def updateUserStatus(self, update, client):
        data = TelegramUserConverter.convert_status(update['status'])
        data['id'] = MESSENGER_TG + str(update['user_id'])
        Event.send(client.account.member, Event.EV_MEMBER_UPDATED, data)

    def updateNewMessage(self, update, client):
        data = TelegramMessageConverter.convert(update['message'], client, with_reply_info=True)
        if data:
            Event.send(client.account.member, Event.EV_MESSAGE_CREATED, data)

    def updateMessageEdited(self, update, client):
        data = {
            'id': update['message_id'],
            'chat_id': MESSENGER_TG + str(update['chat_id']),
            'changedtime': update['edit_date']
        }
        Event.send(client.account.member, Event.EV_MESSAGE_UPDATED, data)

    def updateMessageContent(self, update, client):
        data = {
            'id': update['message_id'],
            'chat_id': MESSENGER_TG + str(update['chat_id']),
        }
        content = TelegramMessageConverter.convert_content(update['new_content'], client, chat_id=update['chat_id'], message_id=update['message_id'])
        if content:
            data.update(content)
            Event.send(client.account.member, Event.EV_MESSAGE_UPDATED, data)

    def updateMessageSendSucceeded(self, update, client):
        data = {
            'id': update['old_message_id'],
            'chat_id': MESSENGER_TG + str(update['message']['chat_id']),
            'sending_state': MESSAGE_SENDING_STATE_SUCCESS,
            'new_id': update['message']['id'],
        }
        Event.send(client.account.member, Event.EV_MESSAGE_UPDATED, data)

    def updateMessageSendFailed(self, update, client):
        data = {
            'id': update['old_message_id'],
            'chat_id': MESSENGER_TG + str(update['message']['chat_id']),
            'sending_state': MESSAGE_SENDING_STATE_FAILED,
            'new_id': update['message']['id'],
        }
        content = TelegramMessageConverter.convert_content(update['nessage']['content'], client)
        data['text'] = content['text'] + "\nSend error: " + update['error_message']
        Event.send(client.account.member, Event.EV_MESSAGE_UPDATED, data)

    def updateDeleteMessages(self, update, client):
        if not update['from_cache']:
            data = {
                'chat_id': MESSENGER_TG + str(update['chat_id']),
                'message_ids': update['message_ids']
            }
            Event.send(client.account.member, Event.EV_MESSAGES_DELETED, data)

    def updateChatReadInbox(self, update, client):
        data = {
            'id': MESSENGER_TG + str(update['chat_id']),
            'incoming_seen_message_id': update['last_read_inbox_message_id'],
            'unread_count': update['unread_count']
        }
        Event.send(client.account.member, Event.EV_CHAT_UPDATED, data)

    def updateChatReadOutbox(self, update, client):
        data = {
            'id': MESSENGER_TG + str(update['chat_id']),
            'outgoing_seen_message_id': update['last_read_outbox_message_id'],
        }
        Event.send(client.account.member, Event.EV_CHAT_UPDATED_OUTBOX, data)

    def updateChatTitle(self, update, client):
        data = {
            'id': MESSENGER_TG + str(update['chat_id']),
            'title': update['title'],
        }
        Event.send(client.account.member, Event.EV_CHAT_UPDATED, data)

    # def updateChatLastMessage(self, update, client):
    #     data = TelegramChatConverter.convert_last_message(update['last_message'])
    #     data['id'] = MESSENGER_TG + str(update['chat_id'])
    #     Event.send(client.account.member, Event.EV_CHAT_UPDATED, data)

    def updateAuthorizationState(self, update, client):
        type = update['authorization_state']['@type']
        if type == 'authorizationStateWaitCode':
            Event.send(client.account.member, Event.EV_TELEGRAM_AUTH_CODE, update['authorization_state']['code_info'])
            return
        if type == 'authorizationStateWaitPassword':
            Event.send(client.account.member, Event.EV_TELEGRAM_AUTH_PASSWORD, update['authorization_state'])
            return
        if type == 'authorizationStateWaitRegistration':
            Event.send(client.account.member, Event.EV_TELEGRAM_AUTH_REGISTRATION, {'tos_text': update['authorization_state']['terms_of_service']['text']['text']})
            return

    def updateTermsOfService(self, update, client):
        Event.send(client.account.member, Event.EV_TELEGRAM_TERMS,
                          {'tos_id': update['terms_of_service_id'],
                           'tos_text': update['terms_of_service']['text']['text']})

    def updateUserChatAction(self, update, client):
        action = TELEGRAM2CHAT_ACTIONS.get(update['action']['@type'], None)
        if action is not None:
            Event.send(client.account.member, Event.EV_CHAT_ACTION,
                          {'member_id': MESSENGER_TG + str(update['user_id']),
                           'chat_id': MESSENGER_TG + str(update['chat_id']),
                           'action': action
                           })

    # def updateFile(self, update, client):
    #     downloaded_file = update['file']['local']['path']
    #     if update['file']['local']['is_downloading_completed'] and downloaded_file:
    #         file_id = update['file']['id']
    #         file_data = self.redis_conn.get("%s:%s:%s" % (self.TG_REDIS_FILE_DOWNLOAD_PREFIX, client.me_id, file_id))
    #         if file_data:
    #             file_data_list = file_data.split(":")
    #             downloadtype = file_data_list[0]
    #             if downloadtype == "msgvideo":
    #                 local_file = client.telegram_get_local_file(TelegramMessageConverter.generate_video_name, update['file'])
    #                 if local_file:
    #                     chat_id = file_data_list[1]
    #                     msg_id = file_data_list[2]
    #                     data = {
    #                         'id': int(msg_id),
    #                         'chat_id': MESSENGER_TG + chat_id,
    #                         'attachment_file': {
    #                             'url': build_absolute_uri(settings.MEDIA_URL + local_file)
    #                         }
    #                     }
    #                     Event.send(client.account.member, Event.EV_MESSAGE_UPDATED, data)
    #                     logging.info("Downloaded file %s -> %s", update['file']['local']['path'], local_file)
    #                 else:
    #                     logging.error("Can't get downloaded file %s" , update['file']['local']['path'])



    # main loop
    def loop(self):
        self.alive = True

        # get initial list of running clients
        while self.alive:
            try:
                gateways = self.get_running_gateways()
            except TimeoutError:
                logger.error("Telegram gateway seems not running... Waiting to start.")
                continue

            self.telegram_gateways_ready = set(gateways)
            break

        self.set_interested_updates()

        logger.info("updates process started. Currently running gateways:{}".format(",".join(map(str, self.telegram_gateways_ready))))

        while self.alive:
            queue_data = self.redis_conn.blpop(self.TG_REDIS_UPDATES_QUEUE, 2)
            now = time.time()
            if queue_data:
                queue, update_str = queue_data
                try:
                    account_id, timestamp, update = json.loads(update_str)
                except Exception as ex:
                    logger.exception("Invalid update: %s", update_str)
                    continue

                if timestamp < (now - 300):
                    logger.error("Telegram update too old and skipped, timestamp:{} now:{}".format(timestamp, now))
                    continue

                if update == 'GATEWAY_STARTED':
                    logger.info("Telegram main gateway process (re)started".format(account_id))
                    self.set_interested_updates()
                    for acc_id in self.telegram_gateways_ready:
                        client = self.get_telegram_client(acc_id)
                        client.start()
                        logger.info("Starting client id={}".format(acc_id))


                elif update == 'INSTANCE_STARTED':
                    self.telegram_gateways_ready.add(account_id)
                    logger.info("[{}] telegram gateway started".format(account_id))
                    client = self.get_telegram_client(account_id)
                    if client:
                        Event.send(client.account.member, Event.EV_TELEGRAM_READY, {})

                elif update == 'INSTANCE_STOPPED':
                    self.telegram_gateways_ready.discard(account_id)
                    if account_id in self.telegram_clients:
                        del self.telegram_clients[account_id]
                    logger.info("[{}] telegram gateway stopped".format(account_id))

                elif account_id in self.telegram_gateways_ready \
                        or update['@type'] == 'updateAuthorizationState' \
                        or update['@type'] == 'updateTermsOfService':
                    client = self.get_telegram_client(account_id)
                    if client:
                        handler = getattr(self, update['@type'], None)
                        if not handler:
                            logger.debug("Telegram Update not handled. %s", update_str)
                        else:
                            try:
                                logger.debug("[%d] Got %s: %s", client.me_id, update['@type'], update)
                                handler(update, client)
                            except Exception as ex:
                                logger.exception("Error in the Telegram update handler: %s", update_str)
                    else:
                        logger.error("Telegram account={} does not found. Can't start client".format(account_id))

                else:
                    logger.debug("Gateway instance not ready yet. Update ignored. %s", update_str)
            else:
                connections['default'].close_if_unusable_or_obsolete()
