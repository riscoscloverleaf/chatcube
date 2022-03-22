try:
    from django.utils.encoding import force_unicode as force_text
except ImportError:  # python3
    from django.utils.encoding import force_text
from django import template
from django.urls import reverse
from django.utils.safestring import mark_safe
from django.templatetags.static import static

from ik.utils.site import build_absolute_uri

register = template.Library()


@register.simple_tag
def absolute_url(url, *args, **kwargs):
    url = reverse(url, args=args, kwargs=kwargs)
    return mark_safe(build_absolute_uri(url))

@register.simple_tag
def absolute_static(var, *args, **kwargs):
    var = var.format(*args, **kwargs)
    return mark_safe(build_absolute_uri(static(var)))

@register.simple_tag
def set(v=None):
    return v
