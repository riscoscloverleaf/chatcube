#!../pythonenv/bin/python
import os
import sys
import django
if __name__ == "__main__":
    sys.path.append(os.path.abspath(os.path.join(os.path.abspath(os.path.split(__file__)[0]), 'apps')))
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "settings.local_prod")
    django.setup()

    from django.core.management import execute_from_command_line

    execute_from_command_line([__name__, 'makemessages',
                               '--ignore','var',
                               '--ignore','log',
                               '--ignore','settings',
                               '--ignore','www'])
    execute_from_command_line([__name__, 'autotranslate'])
    execute_from_command_line([__name__, 'compilemessages'])
    #execute_from_command_line([__name__, 'compilejsi18n'])
    #execute_from_command_line([__name__, 'compilejsi18n',
    #                           '--locale','zh_CN'])



