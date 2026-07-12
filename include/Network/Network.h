#ifndef CHAT_NETWORK_H
#define CHAT_NETWORK_H

#include <stdbool.h>

typedef struct ChatNetwork ChatNetwork;

ChatNetwork* CN_Create(const char* server_addr, int server_port);
void         CN_Destroy(ChatNetwork* cn);

bool         CN_Connect(ChatNetwork* cn);
void         CN_Disconnect(ChatNetwork* cn);
bool         CN_IsConnected(const ChatNetwork* cn);

bool         CN_SendMessage(ChatNetwork* cn, const char* text);
bool         CN_PollReceive(ChatNetwork* cn, char** out_text);

#endif
