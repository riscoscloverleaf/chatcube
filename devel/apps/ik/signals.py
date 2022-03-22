from django.dispatch import Signal

ip_addess_changed = Signal(providing_args=['request', 'old_ip', 'new_ip'])
member_signed_up = Signal(providing_args=['member'])
member_changed = Signal(providing_args=['member'])
member_online_changed = Signal(providing_args=['online'])
member_active_changed = Signal(providing_args=['online'])

# chat
after_send_message = Signal(providing_args=['message'])
before_send_message = Signal(providing_args=['author', 'chat', 'tomember',
                                             'message_body', 'file'])
message_changed =  Signal(providing_args=['message'])
message_seen = Signal(providing_args=['chatmember'])
messages_deleted =  Signal(providing_args=['me', 'chat', 'message_ids', 'unsend'])
chat_history_cleared =  Signal(providing_args=['me', 'chat', 'unsend'])

chatmember_created = Signal(providing_args=['chat', 'member'])
chatmember_deleted = Signal(providing_args=['chat', 'member'])
chatmember_status_changed = Signal(providing_args=['chatmember'])

chat_action = Signal(providing_args=['member', 'chat_id', 'action'])
chat_changed = Signal(providing_args=['chat'])