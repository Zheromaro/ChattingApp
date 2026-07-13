#ifndef CHAT_PROTOCOL_H
#define CHAT_PROTOCOL_H

#include <stdbool.h>
#include <stddef.h>

/* Forward declarations */
struct Message;

/* ---------- Packet types ---------- */
typedef enum {
    PKT_FRIEND_REQ,   /* t:"friend_req" */
    PKT_FRIEND_ACC,   /* t:"friend_acc" */
    PKT_MESSAGE,      /* t:"msg" */
    PKT_PING,         /* t:"ping" (keepalive, optional) */
    PKT_UNKNOWN
} PacketType;

/* ---------- Wire packet ---------- */
typedef struct NetPacket {
    PacketType type;
    char* sender_id;       /* "s" field: our display name / user id */
    char* conversation_id; /* "c" field: conv guid (for msg) */
    char* text;            /* "txt" field: message body */
    char* raw;             /* full JSON string (for debugging/logging) */
} NetPacket;

/* ---------- Serialization ---------- */
char* ChatProtocol_SerializeMessage(const struct Message* msg, const char *conversationID);
char* ChatProtocol_SerializeFriendReq(const char* my_name);
char* ChatProtocol_SerializeFriendAcc(const char* my_name);
char* ChatProtocol_SerializeText(const char* text);

/* ---------- Deserialization ---------- */
NetPacket* ChatProtocol_Deserialize(const char* data);
void       ChatProtocol_FreePacket(NetPacket* pkt);

/* ---------- Helpers ---------- */
PacketType ChatProtocol_ParseType(const char* data); /* peek without full parse */

#endif
