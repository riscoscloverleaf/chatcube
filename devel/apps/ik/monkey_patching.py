import pytz
from django.utils import timezone as orig_timezone
from django.utils.timezone import get_current_timezone, is_aware

# only difference is the is_dst=False default value. This should fix the AmbiguousTimeError exception
def my_make_aware(value, timezone=None, is_dst=False):
    """
    Makes a naive datetime.datetime in a given time zone aware.
    """
    if timezone is None:
        timezone = get_current_timezone()
    if hasattr(timezone, 'localize'):
        # This method is available for pytz time zones.
        return timezone.localize(value, is_dst=is_dst)
    else:
        # Check that we won't overwrite the timezone of an aware datetime.
        if is_aware(value):
            raise ValueError(
                "make_aware expects a naive datetime, got %s" % value)
        # This may be wrong around DST changes!
        return value.replace(tzinfo=timezone)

orig_timezone.make_aware = my_make_aware