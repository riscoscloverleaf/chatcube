from django.contrib import admin
from ik.models import Message, ChatMember, Chat


# @xadmin.sites.register(Attachment)
# class AttachmentAdmin(object):
#     form_layout = (
#         Fieldset("Main", 'file', 'name', 'file_type')
#     )
#
#     list_display = ('id', 'file_tag', 'name', 'file_type')
#     list_per_page = 100
#     search_fields = ('id', 'name')
#     # actions = None
#     use_related_menu = False
#
#     def queryset(self):
#         return Attachment.objects.all()
#
#     def file_tag(self, obj):
#         if obj.attachment.is_image():
#             return "<img src='{}' height=128>".format(obj.thumbnail_url())
#
#         return obj.file
#
#     file_tag.short_description = "file"
#     file_tag.allow_tags = True
#     file_tag.is_column = True
#@admin.site.register(Chat)
class ChatAdmin(admin.ModelAdmin):
    list_display = ('id', 'is_group', 'members_tag')
    list_display_links = ('id',)
    list_per_page = 100
    list_filter = ('members__member',)

    def members_tag(self, obj):
        mems = [str(mem.member) for mem in obj.members.all()]

        return ",".join(mems)

    members_tag.short_description = "Members"
    members_tag.is_column = True


#@admin.site.register(Message)
class MessageAdmin(object):

    list_display = ('id', 'author', 'chat', 'body_tag', 'sendtime')
    list_display_links = ('id',)
    list_per_page = 100
    list_filter = ('author',)
    search_fields = ('id', 'body')
    # actions = None
    use_related_menu = False

    readonly_fields = ('sendtime',)


    def body_tag(self, obj):
        if obj.attachment_photo:
            photo_dict = obj.attachment_photo.as_dict()
            return "<img src='{}' height=128>".format(photo_dict['thumb_url'])

        return obj.message.message_text()

    body_tag.short_description = "body"
    body_tag.allow_tags = True
    body_tag.is_column = True
