import logging
from django.http import Http404
from django.views.generic import TemplateView
from ik.models.members import Member

logger = logging.getLogger("cc")

class UnsubscribeView(TemplateView):
    template_name = "ik/unsubscribed.html"

    def get_context_data(self, **kwargs):
        hash = self.kwargs.get('id')
        try:
            member = Member.objects.get_member_by_hash(hash)
        except Member.DoesNotExist as ex:
            logger.error("{}, hash: {}", ex, hash)
            raise Http404("Member does not exists")

        ctx = {'member': member}
        if member.settings.get_value("unsubscribed") == 1:
            ctx['already_unsubscribed'] = 1
        else:
            member.settings.set_value("unsubscribed", 1)
        return ctx

