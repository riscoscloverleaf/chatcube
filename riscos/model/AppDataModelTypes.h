//
// Created by lenz on 2/6/20.
//

#ifndef ROCHAT_APPDATAMODELTYPES_H
#define ROCHAT_APPDATAMODELTYPES_H
#include <functional>
#include <memory>
#include <vector>

class MessageData;
class ChatData;
class AvatarData;
class StickerGroupData;
class MemberData;
class ChatMemberData;
class MyMemberData;

typedef std::shared_ptr<ChatData> ChatDataPtr;
typedef std::shared_ptr<MemberData> MemberDataPtr;
typedef std::shared_ptr<ChatMemberData> ChatMemberDataPtr;
typedef std::shared_ptr<MyMemberData> MyMemberDataPtr;
typedef std::shared_ptr<MessageData> MessageDataPtr;
typedef std::function<void(const MyMemberDataPtr)> MyMemberCallbackType;
typedef std::function<void(const MemberDataPtr)> MemberCallbackType;
typedef std::function<void(const MessageDataPtr)> MessageCallbackType;
typedef std::function<void(const ChatDataPtr)> ChatCallbackType;
typedef std::function<void(const std::vector<MessageDataPtr>&)> MessagesCallbackType;
typedef std::function<void(const std::vector<MemberDataPtr>&)> MembersCallbackType;

#endif //ROCHAT_APPDATAMODELTYPES_H
