#ifndef CHAT_P2P_H
#define CHAT_P2P_H

#include <stdbool.h>

typedef struct ChatP2P ChatP2P;

typedef enum {
    P2P_EVT_NONE = 0,
    P2P_EVT_FRIEND_REQUEST,
    P2P_EVT_FRIEND_ACCEPT,
    P2P_EVT_MESSAGE,
    P2P_EVT_DISCONNECTED
} P2PEventType;

typedef struct P2PEvent {
    P2PEventType type;
    char* peer_id;
    char* peer_name;
    char* text;
    struct P2PEvent* next;
} P2PEvent;

ChatP2P* P2P_Create(const char* my_display_name, int preferred_port, int* actual_port);
void     P2P_Destroy(ChatP2P* p2p);
bool     P2P_ConnectToPeer(ChatP2P* p2p, const char* addr, int port);
bool     P2P_AcceptFriend(ChatP2P* p2p, const char* peer_id);
bool     P2P_RemovePeer(ChatP2P* p2p, const char* peer_id);
bool     P2P_SendMessage(ChatP2P* p2p, const char* peer_id, const char* text);
bool     P2P_PollEvent(ChatP2P* p2p, P2PEvent** out_event);
void     P2P_FreeEvent(P2PEvent* ev);
int      P2P_GetListenPort(const ChatP2P* p2p);

#endif
