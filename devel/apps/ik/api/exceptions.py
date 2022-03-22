import logging

from django.core.exceptions import ValidationError, PermissionDenied
from django.http import Http404
from django.utils.translation import ugettext as _
from rest_framework.response import Response
from rest_framework import exceptions
from rest_framework.views import exception_handler, set_rollback
from rest_framework import status

logger = logging.getLogger("ik.api.exception")

def ik_exception_handler(exc, context):
    # handle django Validation Errors
    if isinstance(exc, ValidationError):
        if hasattr(exc, "error_dict"):
            err = dict(((k, {'message': er.message, 'code': er.code})
                  for k, er in exc.error_dict.items()))

            if "non_field_errors" in err:
                err = err["non_field_errors"]

            data = {"validation_error": err}
        else:
            data = {"validation_error": [{"message": er.message, "code": er.code}
                                         for er in exc.error_list]}
        set_rollback()
        logger.warning("Validation error: %s", exc, exc_info=1)
        return Response(data, status=status.HTTP_400_BAD_REQUEST)

    if isinstance(exc, exceptions.ValidationError):
        data = {"validation_error": exc.get_full_details()}
        if "non_field_errors" in data["validation_error"]:
            data["validation_error"] = data["validation_error"]["non_field_errors"]
        set_rollback()
        logger.warning("Validation error: %s", exc, exc_info=1)
        return Response(data, status=exc.status_code)

    if isinstance(exc, Http404):
        exc = exceptions.NotFound()
    elif isinstance(exc, PermissionDenied):
        exc = exceptions.PermissionDenied()

    if isinstance(exc, exceptions.APIException):
        headers = {}
        if getattr(exc, 'auth_header', None):
            headers['WWW-Authenticate'] = exc.auth_header
        if getattr(exc, 'wait', None):
            headers['Retry-After'] = '%d' % exc.wait

        if isinstance(exc.detail, (list, dict)):
            data = exc.detail
        else:
            data = exc.get_full_details()

        set_rollback()
        logger.warning("API exception: %s", exc, exc_info=1)
        return Response(data, status=exc.status_code, headers=headers)

    logger.exception("Unhandled exception in API: %s", exc)
    set_rollback()
    return Response({'message':_("There's been an error. It's been reported to the site administrators via e-mail and should be fixed shortly. Thanks for your patience.")+"\n"+str(exc)},
                    status=status.HTTP_500_INTERNAL_SERVER_ERROR)
