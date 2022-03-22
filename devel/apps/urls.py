import importlib
from time import time

import django.views.static
from django.contrib import admin
from django.conf import settings
from django.conf.urls import url, include
from django.conf.urls.i18n import i18n_patterns
from django.urls import path, re_path

from ik.views import m
# from ik.iktranslator.views import UpdateTranslationsView

# urlpatterns = [
#     path('admin/', include(admin.site.urls)),
# ]

urlpatterns = [
    path('errorlogging/', include('ik.errorlogging.urls', namespace="errorlogging")),
    path('m/', m.MView.as_view()),
    path('m/index.html', m.MView.as_view()),
    re_path(r'^m/v[0-9.-]+/$', m.MView.as_view()),
    re_path(r'^m/v[0-9.-]+/index.html$', m.MView.as_view()),
    # url(r'^update-translations/$', UpdateTranslationsView.as_view(), name="update_translations"),
    re_path(r'email/', include('templated_email.urls', namespace='templated_email')),
]

urlpatterns += i18n_patterns(
    path(r'api/', include('ik.api_urls', namespace="api")),
#    url(r'^app/', include('ik.urls', namespace='app')),
    path(r'cq/', include('ik.urls', namespace='ik')),
)

if settings.DEBUG:
    # static files (images, css, javascript, etc.)
    urlpatterns += [
        url(r'^media/(?P<path>.*)$', django.views.static.serve, {
        'document_root': settings.MEDIA_ROOT})
    ]
