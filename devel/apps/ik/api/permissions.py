from rest_framework.permissions import BasePermission
from rest_framework.exceptions import PermissionDenied
from ik import constants

class IsAuthenticated(BasePermission):
    """
    Allows access only to authenticated users.
    """

    def has_permission(self, request, view):
        if request.method == 'OPTIONS':
            return True

        user = request.user
        if user and user.is_authenticated:
            return True
        else:
            return False
