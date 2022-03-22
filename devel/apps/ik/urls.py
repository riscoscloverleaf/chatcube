from django.conf.urls import url
from .views import  unsubscribe

app_name = 'ik'

urlpatterns = [
    url(r'^unsubscribe/(?P<id>\w+)$', unsubscribe.UnsubscribeView.as_view(), name='unsubscribe'),
]
