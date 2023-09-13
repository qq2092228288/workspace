#ifndef TOPICNS_H
#define TOPICNS_H

#include <QObject>

namespace TopicNs {
Q_NAMESPACE
enum class PrimaryTopic
{
    request,                    // client request to server
    append,                     // admin publish append message to server
    update,                     // admin publish update message to server
    remove,                     // admin publish remove message to server
    select                      // admin select database to server
};
Q_ENUM_NS(PrimaryTopic)
enum class ResponseTopic
{
    response                    // server response to client
};
Q_ENUM_NS(ResponseTopic)
enum class SecondaryTopic
{
    deviceInfo,                 // client/server request/response device info
    uploadData,                 // client/server request/response upload data
    software,                   // client/server request/response software data
    signIn,                     // admin/server request/response sign in
    changePassword,             // admin request/response change password
    reports,                    // admin/server request/response reports
    device,                     // admin append/update/delete device
#if ENABLE_COMBINE_DEVICE
    computer,                   // admin append/update/delete computer
#endif
#if ENABLE_COMBINE_DEVICE
    combinedDevice,             // admin append/update/delete combined device
#endif
    allocatedConsumables,       // admin append allocated consumables
    place,                      // admin append/update/delete place
    agent,                      // admin append/update/delete agent
    consultation                // admin/server request/response report consultation information
};
Q_ENUM_NS(SecondaryTopic)
enum class UserStatus
{
    passwordCorrect,
    passwordError,
    passwordChanged,
    passwordModificationFailed
};
Q_ENUM_NS(UserStatus)
enum class RemarksType
{
    newPassword,
    oldPassword,
    mtime
};
Q_ENUM_NS(RemarksType)
};

#endif // TOPICNS_H
