#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <stddef.h>
#include <stdbool.h>

typedef struct Conversation Conversation;
typedef struct User User;
typedef struct Message Message;

Conversation* ConvCreate(void);
void ConvDestroy(Conversation* conv);

bool ConvAddParticipant(Conversation* conv, User* user);
bool ConvAddMessage(Conversation* conv, Message* msg);
const User* ConvFindParticipant(const Conversation* conv, const char* user_id);
const char* ConvGetID(const Conversation* conv);
size_t ConvGetMessageCount(const Conversation* conv);
const Message* ConvGetMessage(const Conversation* conv, size_t index);

#endif
