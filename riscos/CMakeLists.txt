cmake_minimum_required(VERSION 3.15)

# this one is important
SET(CMAKE_SYSTEM_NAME GNU)

set(CMAKE_SYSTEM_PROCESSOR arm)

set(riscosgcc /home/lenz/gccsdk)
set(CMAKE_C_COMPILER ${riscosgcc}/env/arm-unknown-riscos-gcc)
set(CMAKE_CXX_COMPILER ${riscosgcc}/env/arm-unknown-riscos-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

project(rochat)
include_directories("libs/eventbus/include" "libs/tbx" "libs/utf8" "libs/librufl/include" "libs/cloverleaf" "libs/inipp")
set(CURL_INCLUDE_DIR ${riscosgcc}/env/include)
set(CURL_LIBRARY ${riscosgcc}/env/lib)

add_subdirectory("libs/tbx/tbx")
add_subdirectory("libs/librufl/src")

add_executable(rochat
        libs/cJSON/cJSON.c
        libs/cloverleaf/cloverleaf/CLException.cpp
        libs/cloverleaf/cloverleaf/IdleTask.cpp
        libs/cloverleaf/cloverleaf/CLGraphics.cpp
        libs/cloverleaf/cloverleaf/CLImage.cpp
        libs/cloverleaf/cloverleaf/CLImagePNGLoader.cpp
        libs/cloverleaf/cloverleaf/CLImageJPGLoader.cpp
        libs/cloverleaf/cloverleaf/CLImageLoader.cpp
        libs/cloverleaf/cloverleaf/CLImageCache.cpp
        libs/cloverleaf/cloverleaf/Logger.cpp
        libs/cloverleaf/cloverleaf/CLUtf8.cpp
        libs/cloverleaf/cloverleaf/rosprite.cpp
        libs/cloverleaf/cloverleaf/CLSound.cpp
        libs/cloverleaf/cloverleaf/CLClipboard.cpp
        service/CLHTTPService_v2.cpp
        service/IKConfig.cpp
        model/AppDataModel.cpp
        model/AppDataModelUpdates.cpp
        model/AppDataModelTelegram.cpp
        model/AppDataModelLoads.cpp
        model/AppDataModelAuth.cpp
        model/AvatarData.cpp
        model/AppEvents.cpp
        model/ChatData.cpp
        model/FileCacheDownloader.cpp
        model/JsonData.cpp
        model/MemberData.cpp
        model/MessageData.cpp
        model/NetworkRequests.cpp
        model/StickerData.cpp
        model/ChoicesModel.cpp
        model/TelegramData.cpp
        model/ChatMemberData.cpp
        ui/ChatMainUI.cpp
        ui/ChatListView.cpp
        ui/MessagesListView.cpp
        ui/LoginDialog.cpp
        ui/AppMenu.cpp
        ui/AppIconbar.cpp
        ui/FontStyles.cpp
        ui/EditProfileSettings.cpp
        ui/SelectAvatarView.cpp
        ui/TelegramAuthCodeDialog.cpp
        ui/TelegramRegisterOrChangePhoneDialog.cpp
        ui/TelegramRegistrationDialog.cpp
        ui/TelegramAcceptTOSDialog.cpp
        ui/RegistrationDialog.cpp
        ui/SendStickerUI.cpp
        ui/ScreenshootUI.cpp
        ui/ResetPasswordDialog.cpp
        ui/CreateChatDialog.cpp
        ui/AvatarChanger.cpp
        ui/ForwardMessageDialog.cpp
        ui/RecipinetList.cpp
        ui/TelegramPasswordDialog.cpp
        ui/ProgressBarUI.cpp
        ui/ManageGroupDialog.cpp
        ui/ChatMembersList.cpp
        ui/AppColours.cpp
        ui/AddMembersInChatDialog.cpp
        ui/LogoutDialog.cpp
        ui/ViewProfileDialog.cpp
        ui/DeleteAccountDialog.cpp
        ui/FeedbackDialog.cpp
        main.cpp
        utils.cpp
        AppEventHandlers.cpp
        )
target_link_libraries (rochat curl crypto z ssl ssh2 OSLib32-soft OSLibSupport32-soft png jpeg tbx rufl)

add_custom_target(RunImage COMMAND ${riscosgcc}/cross/bin/elf2aif rochat !RunImage,ff8
        DEPENDS rochat)

add_executable(updater
        updater.cpp
        utils.cpp
        libs/cloverleaf/cloverleaf/Logger.cpp)

target_link_libraries (updater OSLib32 tbx)

add_custom_target(UpdaterRunImage COMMAND ${riscosgcc}/cross/bin/elf2aif updater updater,ff8
        DEPENDS updater)
