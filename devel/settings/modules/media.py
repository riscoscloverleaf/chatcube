import os
from settings.globalconf import PROJECT_DIR, APP_TEST_MODE

# Static files (CSS, JavaScript, Images)
# https://docs.djangoproject.com/en/1.8/howto/static-files/
WWW_DIR = os.path.join(PROJECT_DIR, 'www')


MEDIA_URL = '/media/'
MEDIA_ROOT = os.path.join(WWW_DIR,'media')
FILE_UPLOAD_PERMISSIONS = 0o644
FILE_UPLOAD_DIRECTORY_PERMISSIONS = 0o755
FILE_UPLOAD_TEMP = 'temp_uploads'
FILE_UPLOAD_TEMP_DIR = os.path.join(MEDIA_ROOT, FILE_UPLOAD_TEMP)

MEMBERS_UPLOADS_DIR = os.path.join(MEDIA_ROOT, 'members', 'uploads')
MEMBERS_UPLOADS_URL = '/media/members/uploads/'

STATIC_URL = '/static/'
STATIC_ROOT = os.path.join(WWW_DIR, 'static')
STATICFILES_DIRS = (
    os.path.join(PROJECT_DIR, 'static'),
)

STATICFILES_FINDERS = (
    'django.contrib.staticfiles.finders.FileSystemFinder',
    'django.contrib.staticfiles.finders.AppDirectoriesFinder',
#    'compressor.finders.CompressorFinder',
)

COMPRESS_CSS_FILTERS = ['compressor.filters.css_default.CssAbsoluteFilter',
                        'compressor.filters.cssmin.CSSMinFilter',
                        ]

# model name must be lowercase, same as in django_contenttype table
# IMGCROPPER_FORMS = {
#     'myapp.memberphoto': 'myapp.forms.photos.PhotoCropForm'
# }

#THUMBNAIL_DBM_FILE = os.path.join(PROJECT_DIR, 'var','global', 'thumbnail.db')
#THUMBNAIL_KVSTORE = 'sorl.thumbnail.kvstores.dbm_kvstore.KVStore'

THUMBNAIL_BASEDIR = "thumbs"
THUMBNAIL_PROGRESSIVE = 100
THUMBNAIL_QUALITY = 85
if APP_TEST_MODE:
    THUMBNAIL_TRANSPARENCY_EXTENSION = 'png'
    THUMBNAIL_EXTENSION = 'jpg'
else:
    THUMBNAIL_TRANSPARENCY_EXTENSION = 'png'
    THUMBNAIL_EXTENSION = 'png'

#THUMBNAIL_TRANSPARENCY_EXTENSION = 'jpg'
THUMBNAIL_NAMER = 'easy_thumbnails.namers.hashed'
#THUMBNAIL_SUBDIR = 'thumbs'
THUMBNAIL_ALIASES = {
    '' : {
        'fullsize': {'size': (1920,1080), 'subsampling': 1},
        'attachment': {'size': (150,150)},
        'small_profile': {'size': (56, 56), 'crop': True},
    }
}
FULL_THUMBNAIL_CONF = THUMBNAIL_ALIASES['']['fullsize']
ATTACHMENT_THUMBNAIL_CONF = THUMBNAIL_ALIASES['']['attachment']
SMALL_PROFILE_THUMBNAIL_CONF = THUMBNAIL_ALIASES['']['small_profile']

# PROFILE_PHOTO_ASPECT = float(PROFILE_THUMBNAIL_CONF['size'][0]) / float(PROFILE_THUMBNAIL_CONF['size'][1])
#
# PHOTO_ASPECT_TOLERANCE = 0.07
PHOTO_MIN_WIDTH=200
PHOTO_MIN_HEIGHT=200

