import logging
from rest_framework.exceptions import NotFound

logger = logging.getLogger("cc")


class IKAbstractChatClient:
    MESSAGES_FILTER_ATTACHMENTS = 1
    MESSAGES_FILTER_LINKS = 2
    MESSAGES_FILTER_EMAILS = 3

    MESSAGES_LOAD_PREV = 0
    MESSAGES_LOAD_NEXT = 1

    def __init__(self, me):
        self.me = me

    def get_or_create_private_chat(self, member_id, full_info=True):
        raise NotImplementedError()

    def create_group_chat(self, member_ids, title):
        raise NotImplementedError()

    def send_chat_action(self, chat_id: str, action: int):
        raise NotImplementedError()

    def check_send_permissions(self, chat) -> None:
        raise NotImplementedError()

    def send_message_text(self, chat, text, reply_to_id=None):
        raise NotImplementedError()

    def send_message_file(self, chat, file, file_name=None, file_type=None, caption=None, reply_to_id=None):
        raise NotImplementedError()

    def send_message_sticker(self, chat, sticker, reply_to_id=None):
        raise NotImplementedError()

    def send_message_photo(self, chat, file, caption=None, reply_to_id=None):
        raise NotImplementedError()

    def edit_message(self, chat_id, message_id, text):
        raise NotImplementedError()

    def forward_message(self, to_chat_ids, from_chat_id, message_id):
        raise NotImplementedError()

    def delete_messages(self, chat_id, message_ids, unsend):
        raise NotImplementedError()

    def delete_chat_history(self, chat_id, remove_from_chat_list, unsend):
        raise NotImplementedError()

    def add_members_to_chat(self, chat, member_ids):
        raise NotImplementedError()

    def join_chat(self, chat_id):
        raise NotImplementedError()

    def leave_chat(self, chat):
        raise NotImplementedError()

    def get_user_as_dict(self, user, raise_not_found_exception=True):
        raise NotImplementedError()

    def get_users_as_dict(self, user_ids, raise_not_found_exception=True):
        raise NotImplementedError()

    def get_chat(self, chat_id, full_info=True):
        raise NotImplementedError()

    def get_chat_as_dict(self, chat_id):
        raise NotImplementedError()

    def get_chat_history(self, chat_id, from_message_id=0, limit=20, filter=None, offset=0):
        raise NotImplementedError()

    def get_whole_chat_history_queryset(self, chat_id):
        raise NotImplementedError()

    def get_message(self, chat_id, message_id):
        raise NotImplementedError()

    def get_chats_as_dict(self):
        raise NotImplementedError()

    def get_chat_members(self, chat_id, search, limit):
        raise NotImplementedError()

    def mark_seen(self, chat_id, message_id):
        raise NotImplementedError()

    def open_chat(self, chat_id):
        return

    def get_contacts(self, except_in_chat=None):
        return []

    def search_chats(self, query):
        return []

    def search_chat_messages(self, chat_id, query, filter, from_message_id=0, limit=20, sender_id=None):
        return [], False, False

    def set_chat_title(self, chat_id, title):
        raise NotImplementedError()

    def set_chat_photo(self, chat_id, photo_file):
        raise NotImplementedError()

_chat_client_classes = {}

def register_chat_client(messenger, client):
    _chat_client_classes[messenger] = client

def get_chat_client(messenger, me, raise_exception=True) -> IKAbstractChatClient:
    klass = _chat_client_classes.get(messenger)
    if klass:
        return klass.get_client(me, raise_exception=raise_exception)
    else:
        msg = "Messenger id '{}' not registered. ".format(messenger)
        logger.error(msg)
        raise NotFound(msg)
