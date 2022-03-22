from django.shortcuts import HttpResponse
from django.views.generic import View


class RobotsTxtView(View):

    def get(self, request, *args, **kwargs):
        content = """
User-agent: *
Crawl-delay: 1
"""
        return HttpResponse(content, content_type="text/plain")
