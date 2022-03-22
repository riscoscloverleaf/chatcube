from django_globals import globals

def current_request():
    """
    Retrieves the request from the current thread.
    """
    return getattr(globals, "request", None)

def set_current_request(request):
    globals.request = request

def current_user():
    req = current_request()
    if req:
        return req.user
    return None

