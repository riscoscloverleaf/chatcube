from rest_framework.renderers import JSONRenderer

class HTMLJSONRenderer(JSONRenderer):
    media_type = 'text/html'
