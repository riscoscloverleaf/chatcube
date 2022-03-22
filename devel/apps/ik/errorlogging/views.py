# -*- coding: utf-8 -*-
import logging
import json
from django.conf import settings
from django.views.generic.base import TemplateView
from django.views.decorators.csrf import csrf_exempt
from django.utils.decorators import method_decorator
from django.utils.timezone import now
from ik.utils.ip import current_ip
from .settings import LOGGER_NAME, ENABLE_LOGGER, LOG_FORMAT

logger = logging.getLogger(LOGGER_NAME)

class LoggingView(TemplateView):
    """ Add Log
    """
    http_method_names = ['post']
    template_name = 'ik/errorlogging/jslogging.html'

    @method_decorator(csrf_exempt)
    def dispatch(self, request, *args, **kwargs):
        return super(LoggingView, self).dispatch(request, *args, **kwargs)

    def get_logger_message(self, data):
        return LOG_FORMAT.format(**data)

    def post(self, request, *args, **kwargs):
        errors_str = request.POST.get('errors')
        try:
            errors = json.loads(request.POST['errors'])
        except Exception:
            logger.error(u"Failed to decode json: {}".format(errors_str))
            context = {'numerrors': 1}
            return self.render_to_response(context)

        for err in errors:
            err['created_at'] = now()
            err['user'] = request.user
            err['session_key'] = request.session.session_key or ''
            err['remote_addr'] = current_ip(request)
            err['level'] = err['level'].upper()
            err['ver'] = err.get('ver', '0.0.0')
            err['site'] = err.get('site', '--')
            err['platform'] = err.get('platform', '')
            if not 'member' in err:
                if request.user.is_authenticated:
                    err['member'] = "{}:{}".format(settings.SITE_CODE, request.user.userid)
                else:
                    err['member'] = '-'
            if ENABLE_LOGGER:
                logger.error(LOG_FORMAT.format(**err))
        context = {'numerrors': len(errors)}
        return self.render_to_response(context)
