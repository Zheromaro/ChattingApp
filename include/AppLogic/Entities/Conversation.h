#ifndef CONVERSATION_H
#define CONVERSATION_H

#include "AppLogic/Entities/Message.h"
#include "AppLogic/Entities/User.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct Conversation Conversation;

Conversation* ConvCreate(void);
void ConvDestroy(Conversation* conv);

bool ConvAddParticipant(Conversation* conv, User* user);
bool ConvAddMessage(Conversation* conv, Message* msg);
const User* ConvFindParticipant(const Conversation* conv, const char* user_id);
const char* ConvGetID(const Conversation* conv);

#endif
