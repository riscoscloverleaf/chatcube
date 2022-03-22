CACHES = {
    "default": {
        "BACKEND": "django_redis.cache.RedisCache",
        # default location may be overrided in the site_CODE.py
        "LOCATION": "redis://127.0.0.1:6391/1",
        "OPTIONS": {
            "CLIENT_CLASS": "django_redis.client.DefaultClient",
            "COMPRESS_MIN_LEN": 20
        }
    },
    "messengers": {
        "BACKEND": "django_redis.cache.RedisCache",
        # default location may be overrided in the site_CODE.py
        "LOCATION": "redis://127.0.0.1:6391/2",
        "OPTIONS": {
            "CLIENT_CLASS": "django_redis.client.DefaultClient",
            "COMPRESS_MIN_LEN": 20
        }
    }
}

COMPRESS_CACHE_KEY_FUNCTION = 'ik.utils.compressor.ik_cachekey'