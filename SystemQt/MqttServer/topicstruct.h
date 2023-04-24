#ifndef TOPICSTRUCT_H
#define TOPICSTRUCT_H

#include <QObject>


// third level topic is unique/admin id
// there are only three levels of topic at most

// primary topic for admin, client and server
#define TOPIC_1_ACS_REQ                     "req"                   // client request to server
#define TOPIC_1_ACS_RESP                    "resp"                  // server response to client
#define TOPIC_1_AS_APPEND                   "append"                // admin publish append message to server
#define TOPIC_1_AS_UPDATE                   "update"                // admin publish update message to server
#define TOPIC_1_AS_DELETE                   "delete"                // admin publish delete message to server
// secondary topic for client and server
#define TOPIC_2_CS_DEVICE_INFO              "deviceinfo"            // client/server request/response device info
#define TOPIC_2_CS_UPLOAD_DATA              "upload"                // client/server request/response upload data
#define TOPIC_2_CS_SOFTWARE                 "software"              // client/server request/response software data
// secondary topic for admin and server
#define TOPIC_2_AS_SIGN_IN                  "signin"                // admin/server request/response sign in
#define TOPIC_2_AS_PASSWORD                 "password"              // admin update password
#define TOPIC_2_AS_DEVICE                   "device"                // admin append/update/delete device
#define TOPIC_2_AS_COMPUTER                 "computer"              // admin append/update/delete computer
#define TOPIC_2_AS_COMBINED_DEVICE          "combineddevice"        // admin append/update/delete combined device
#define TOPIC_2_AS_PLACE                    "place"                 // admin append/update/delete place
#define TOPIC_2_AS_AGENT                    "agent"                 // admin append/update/delete agent
#define TOPIC_2_AS_REPORTS                  "reports"               // admin/server request/response reports
#define TOPIC_2_AS_SOFTWARE                 "software"              // admin append/update/delete software
#define TOPIC_2_AS_ALLOCATED_CONSUMABLES    "allocatedconsumables"  // admin append allocated consumables

#endif // TOPICSTRUCT_H
