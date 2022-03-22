AUTH_USER_MODEL = 'ik.Member'
ACCOUNT_PASSWORD_MIN_LENGTH = 5

AUTHENTICATION_BACKENDS = (
#    'django.contrib.auth.backends.ModelBackend',
    'ik.auth_backends.IKAuthenticationBackend',
)
