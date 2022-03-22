import logging
import time

from django.core.management.base import BaseCommand

from ik.models.members import Member
from ik.utils.email import can_send_email_to, send_email_template

logger = logging.getLogger("cc")

class Command(BaseCommand):
    help = "Send email from some template to members"

    def send_to_member(self, member, template_name, force):
        if force or can_send_email_to(member):
            self.stdout.write(self.style.SUCCESS('Send to member %s' % member))
            ctx = {
                "member": member,
            }
            send_email_template(template_name + ".html", ctx, member.email, "cc_template_"+template_name, member=member)
            member.settings.set_value("sent_"+template_name, int(time.time()))

    def add_arguments(self, parser):
        parser.add_argument('--email', type=str, help="Email of member")
        parser.add_argument('--template', type=str, help="Template name")
        parser.add_argument('--send-to-all', action='store_true', help="Send to all members")
        parser.add_argument('--resend', action='store_true', help="Resend even sent already")
        parser.add_argument('--force', action='store_true', help="Force send even unsubscribed")

    def handle(self, *args, **options):
        email = options['email']
        template = options['template']

        if not template:
            self.stdout.write(self.style.SUCCESS('Must specify template name to send'))
            return

        if email:
            member = Member.objects.get(email=email)
            self.send_to_member(member, template, options.get('force', False))
            return

        if options['send_to_all']:
            qs = Member.objects.filter(is_active=True, is_staff=False).exclude(settings__name="unsubscribed")
            if not options.get('resend', False):
                qs = qs.exclude(settings__name="sent_" + template)
            for m in qs:
                self.send_to_member(m, template, options.get('force', False))
