//
// Created by lenz on 2/3/20.
//

#ifndef ROCHAT_APPEVENTS_H
#define ROCHAT_APPEVENTS_H
#include <string>
#include <eventbus/EventBus.h>
#include "AppDataModelTypes.h"
#include "TelegramData.h"
#include "../service/CLHTTPService_v2.h"

#define CHAT_ACTION_CANCEL 0
#define CHAT_ACTION_TYPING 1

namespace AppEvents {
    struct InitialDataLoaded {};

    struct ProgressBarControl {
        int percent_done = 0;
        os_t estimated_time = 0;
        int estimated_progress = 0;
        CLHTTPRequest *req = nullptr; // must specify req or download_url;
        std::string download_url;
        std::string label;
    };

    struct DownloadingFilesProgress {
        int percent_done = 0;
    };
    struct UploadingProgress {
        CLHTTPRequest *req = nullptr;
        int percent_done = 0;
    };
    struct DownloadingProgress {
        CLHTTPRequest *req = nullptr;
        int percent_done = 0;
    };
    struct LoggedIn {};
    struct SignedUp {};
    struct LoginRequired {};
    struct ConnectionStateChanged {
        bool connected;
    };

    struct ChatListLoaded {};
    struct OpenedChatChanged {
        ChatDataPtr chat;
    };
    struct ChatAdded {
        ChatDataPtr chat;
    };
    struct ChatChanged {
        ChatDataPtr chat;
        bool ordering_changed;
        unsigned int changes;
    };
    struct ChatCleared {
        ChatDataPtr chat;
    };
    struct ChatDeleted {
        ChatDataPtr chat;
    };
    struct ChatPicSmallChanged {
        ChatDataPtr chat;
    };
    struct MessageAdded {
        ChatDataPtr chat;
        MessageDataPtr msg;
    };
    struct MessageChanged {
        ChatDataPtr chat;
        MessageDataPtr msg;
    };
    struct MessageDeleted {
        ChatDataPtr chat;
        MessageDataPtr msg;
    };
    struct MessagesLoaded {
        ChatDataPtr chat;
        bool is_first_load;
    };
    struct MessageSendFailed {
        std::string chat_id;
        MessageData* message;
        std::string error;
    };

    struct MemberLoaded {
        MemberDataPtr mem;
    };
    struct MemberChanged {
        MemberDataPtr mem;
        unsigned int changes;
    };

    struct MyChatcubeAvatarChanged {};
    struct MyTelegramAvatarChanged {};

    struct ShowAlert {
        std::string message;
    };

    struct TelegramAuthCode {
        TelegramAuthCodeData data;
    };

    struct TelegramAuthPassword {
        TelegramAuthPasswordData data;
    };

    struct TelegramAuthRegistration {
        TelegramAuthRegistrationData data;
    };

    struct TelegramTermsOfService {
        TelegramTermsOfServiceData data;
    };

    struct TelegramReady {};

}

extern Dexode::EventBus g_app_events;
#endif //ROCHAT_APPEVENTS_H
