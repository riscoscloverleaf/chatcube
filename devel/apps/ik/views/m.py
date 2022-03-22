import os
from django.conf import settings
from django.shortcuts import HttpResponseRedirect
from django.http.response import FileResponse
from django.views.generic import View

class MView(View):
    def get(self, request, *args, **kwargs):
        prod_path = os.path.realpath(os.path.join(settings.PROJECT_DIR, '..', 'm','prod'))
        release_name = os.path.basename(prod_path)
        if release_name in request.path:
            prod_path = os.path.join(prod_path, "index.html")
            return FileResponse(open(prod_path, 'rb'), content_type='text/html')
        else:
            query_str = request.META['QUERY_STRING']
            query_str = query_str.replace('&',';')
            return HttpResponseRedirect("/m/{}/index.html#{}".format(release_name, query_str))
