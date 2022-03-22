from rest_framework import serializers
from rest_framework.exceptions import ValidationError
from ik.account_adapter import IKAccountAdapter
from ik.messengers.telegram.models import TelegramAccount

class TelegramRegisterPhoneSerializer(serializers.Serializer):
    phone = serializers.CharField(required=True)

    def validate_phone(self, val):
        phone = IKAccountAdapter.normalize_phone(val)
        if TelegramAccount.objects.filter(phone=phone).exists():
            raise ValidationError("Telegram phone number {} already registered".format(phone))
        return phone


class TelegramRegisterMemberSerializer(serializers.Serializer):
    first_name = serializers.CharField(required=True)
    last_name = serializers.CharField(required=False)


class MyProfileTelegramAccountSerializer(serializers.ModelSerializer):
    first_name = serializers.CharField(required=False)
    last_name = serializers.CharField(required=False)
    pic = serializers.CharField(required=False)
    username = serializers.CharField(required=False)
    avatar = serializers.CharField(required=False, write_only=True)
    profile_image = serializers.ImageField(required=False, write_only=True)

    def update(self, instance, validated_data):
        instance.update_my_data(validated_data.get('first_name', instance.first_name()),
                             validated_data.get('last_name', instance.last_name()),
                             validated_data.get('username', instance.username()),
                             validated_data.get('profile_image'),
                             validated_data.get('avatar')
                             )

        return instance


    class Meta:
        model = TelegramAccount
        fields = ('phone','tg_user_id', 'first_name', 'last_name', 'username', 'pic', 'avatar', 'profile_image')
        read_only_fields = ('phone','tg_user_id')