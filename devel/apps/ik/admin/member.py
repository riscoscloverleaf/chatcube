from django.contrib import admin
from django.conf import settings
from ik.models.members import Member


#@admin.site.register(Member)
class MemberAdmin(admin.ModelAdmin):
    list_display = ('id', 'thumbnail_profile_tag',
                    'displayname', 'userid', 'email', 'phone', 'date_joined')
    list_display_links = ('id',)
    ordering = ('-date_joined',)
    filter_horizontal = []
    list_per_page = 100
    list_gallery = True
    search_fields = ('id', 'displayname', 'email', 'userid')

    exclude = ('id', 'groups')


    def thumbnail_profile_tag(self, obj):
        if obj.profile_image:
            return u'<a target="_blank" href="%s" data-gallery="gallery"><img src="%s" /></a>' % (obj.pic,
                                                                           obj.profile_photo.get_thumbnail(settings.SMALL_PROFILE_THUMBNAIL_CONF).url)
        else:
            return 'No-Photo'

    thumbnail_profile_tag.short_description = "Profile"
    thumbnail_profile_tag.allow_tags = True
    thumbnail_profile_tag.is_column = True
    thumbnail_profile_tag.allow_export = False
