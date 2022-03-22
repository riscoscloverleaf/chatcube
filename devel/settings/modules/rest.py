REST_FRAMEWORK = {
    'EXCEPTION_HANDLER': 'ik.api.exceptions.ik_exception_handler',
    'DEFAULT_AUTHENTICATION_CLASSES': (
        'ik.api.rest_authentication.IKTokenOrSessionMiddlewareAuthentication',
    ),
    'DEFAULT_RENDERER_CLASSES': (
        'ik.api.renderer.IKJSONRenderer',
        'rest_framework.renderers.TemplateHTMLRenderer'
    ),
    'DATE_INPUT_FORMATS': ('%d-%m-%Y','iso-8601'),
}

REST_USE_JWT = False

REST_AUTH_SERIALIZERS = {
    'USER_DETAILS_SERIALIZER': 'ik.api.serializers.profile.MyProfileSerializer',
    'LOGIN_SERIALIZER': 'ik.api.serializers.auth.LoginSerializer'
}
