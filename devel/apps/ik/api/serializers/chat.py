from rest_framework import serializers

class SendMessageSerializer(serializers.Serializer):
    recipient_id = serializers.CharField(required=False)
    chat_id = serializers.CharField(required=False)
    type = serializers.IntegerField(required=True)
    text = serializers.CharField(required=False, trim_whitespace=False)
    file = serializers.FileField(required=False)
    file_type = serializers.IntegerField(required=False)
    file_name = serializers.CharField(required=False)
    reply_to_id = serializers.IntegerField(required=False)


class CreateGroupChatSerializer(serializers.Serializer):
    title = serializers.CharField(required=True)
    member_ids = serializers.CharField(required=True)


class CreatePrivateChatSerializer(serializers.Serializer):
    member_id = serializers.CharField(required=True)


class ForwardMessageSerializer(serializers.Serializer):
    msg_id = serializers.IntegerField(required=True)
    chat_ids = serializers.CharField(required=True)
