from rest_framework.authentication import BaseAuthentication, CSRFCheck
from rest_framework.exceptions import PermissionDenied

class IKTokenOrSessionMiddlewareAuthentication(BaseAuthentication):
    """
    Use Django's session framework for authentication.
    """

    def authenticate(self, request):
        # Get the user and AuthorizedDevice instance from the underlying HttpRequest object
        user = getattr(request._request, 'user', None)
        auth = getattr(request._request, 'auth', None)
        if not user or not user.is_active:
            return None

        if not auth: # the authorizedDevide missing, it is mean browser session Auth
            self.enforce_csrf(request)

            # CSRF passed with authenticated user
            return (user, None)
        else:
            # pass AuthorizedDevice instance as auth here
            return (user, auth)

    def enforce_csrf(self, request):
        """
        Enforce CSRF validation for session based authentication.
        """
        reason = CSRFCheck().process_view(request, None, (), {})
        if reason:
            # CSRF failed, bail with explicit error message
            raise PermissionDenied('CSRF Failed: %s' % reason)

    def authenticate_header(self, request):
        return 'Token'

