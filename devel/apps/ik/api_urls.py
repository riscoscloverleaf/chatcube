from django.conf.urls import url, include

app_name = "ik.api"

urlpatterns = [
    url(r'^', include('ik.api.api_urls')),
]
