from django.db import models
from ik.models import Member

from ...models import Chat, messenger_participant_models
from ik.constants import MESSENGER_FB

class FacebookParticipant(Participant):
    fb_id = models.BigIntegerField(unique=True)
    nickname = models.CharField(max_length=255)

    @property
    def displayname(self):
        return self.nickname

    class Meta:
        db_table = 'facebook_participants'


class FacebookChat(Chat):
    fb_id = models.BigIntegerField(unique=True)
    class Meta:
        db_table = 'facebook_chats'

messenger_participant_models[MESSENGER_FB] = FacebookParticipant
