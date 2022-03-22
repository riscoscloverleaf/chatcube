# -*- coding: utf-8 -*-
from django.conf.urls import url
from .views import LoggingView

app_name = 'ik'

urlpatterns = [
    url(r'^add/$', LoggingView.as_view(), name="add_log"),
]
