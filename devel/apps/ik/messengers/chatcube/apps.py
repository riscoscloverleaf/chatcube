from django.apps import AppConfig

class IKMessengersChatcubeConfig(AppConfig):
    name = "ik.messengers.chatcube"
    label = 'chatcube'
    verbose_name = 'Chatcube messenger'

    def ready(self):
        import ik.messengers.chatcube.client
