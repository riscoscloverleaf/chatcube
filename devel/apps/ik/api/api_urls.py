from django.conf.urls import url
from .api_views import  auth, utils, profile, chat, telegram

urlpatterns = [
    url(r'^auth/signup/$', auth.SignupMemberView.as_view()),
    url(r'^auth/login/$', auth.LoginMemberView.as_view()),
    url(r'^auth/changepassword/$', auth.ChangePasswordView.as_view()),
    url(r'^auth/sendcode/$', auth.SendCodeView.as_view()),
    url(r'^auth/verifycode/$', auth.VerifyCodeView.as_view()),
    url(r'^auth/resetpassword/$', auth.ResetPasswordView.as_view()),

    url(r'^auth/telegram/phone/$', telegram.TelegramRegisterOrChangePhoneView.as_view()),
    url(r'^auth/telegram/code/$', telegram.TelegramAuthCode.as_view()),
    url(r'^auth/telegram/password/$', telegram.TelegramAuthPassword.as_view()),
    url(r'^auth/telegram/register/$', telegram.TelegramRegisterMemberView.as_view()),
    url(r'^auth/telegram/accepttos/$', telegram.TelegramAcceptTOS.as_view()),
    url(r'^auth/telegram/delete/$', telegram.TelegramDeleteAccount.as_view()),
    url(r'^auth/telegram/unregister/$', telegram.TelegramUnregisterAccount.as_view()),

    url(r'^telegram/me/$', telegram.TelegramGetMe.as_view()),
    url(r'^telegram/download/(?P<file_kind>[a-z]+)/(?P<file_id>\d+)/$', telegram.TelegramDownloadFile.as_view(), name="telegram_download"),

    url(r'^locale-catalog/$', utils.LoadLocaleCatalogView.as_view()),
    url(r'^initial/$', utils.LoadInitialView.as_view()),
    url(r'^log/$', utils.LogLine.as_view()),
    url(r'^avatars/$', utils.AvatarsView.as_view()),
    url(r'^stickers/$', utils.StickersView.as_view()),
    url(r'^settings/(?P<name>\w+)/$', utils.ChangeMemberSettingView.as_view()),

    url(r'^profile/my/$', profile.MyProfileView.as_view()),
    url(r'^profile/my/delete/$', profile.MyProfileDelete.as_view()),
    url(r'^profile/(?P<user_id>[a-zA-Z0-9_-]+)/$', profile.ProfileView.as_view()),
    url(r'^profile/$', profile.ProfilesListView.as_view()),

    url(r'^contacts/(?P<messenger_id>[A-Z])/$', chat.ContactsListView.as_view()),

    url(r'^chat/send/$', chat.SendMessageView.as_view()),
    url(r'^chat/create/private/$', chat.ChatCreatePrivateView.as_view()),
    url(r'^chat/create/group/$', chat.ChatCreateGroupView.as_view()),
    url(r'^chat/search/(?P<messenger_id>[A-Z])/$', chat.SearchChatsView.as_view()),
    url(r'^chat/$', chat.ChatsListView.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/$', chat.ChatView.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/forward/$', chat.ForwardMessage.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/messages/$', chat.ChatConversationView.as_view(), name="chat_conversation"),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/messages/(?P<message_id>\w+)/seen/$', chat.MarkSeenMessageView.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/messages/(?P<message_id>\w+)/edit/$', chat.EditMessageView.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/messages/delete/$', chat.DeleteMessagesView.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/messages/export/$', chat.ChatMessagesExport.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/search/$', chat.SearchChatMessagesView.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/clear/$', chat.DeleteChatHistoryView.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/open/$', chat.ChatOpenView.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/join/$', chat.JoinChatView.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/leave/$', chat.LeaveChatView.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/action/$', chat.SendChatActionView.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/members/$', chat.ChatMembersListView.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/add-members/$', chat.AddMembersToChatView.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/set-photo/$', chat.SetChatPhoto.as_view()),
    url(r'^chat/(?P<chat_id>[a-zA-Z0-9_-]+)/set-title/$', chat.SetChatTitle.as_view()),

    # url('^chat/(?P<chat>\w+)/(?P<message_id>\w+)/change/$', chat.ChangeMessageView.as_view()),

]