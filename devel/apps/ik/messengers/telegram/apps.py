from django.apps import AppConfig

class IKMessengersTelegramConfig(AppConfig):
    name = "ik.messengers.telegram"
    label = 'telegram'
    verbose_name = 'Telegram messenger'

    def ready(self):
        import ik.messengers.telegram.client
        import ik.messengers.telegram.listeners
