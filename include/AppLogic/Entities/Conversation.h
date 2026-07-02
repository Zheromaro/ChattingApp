#ifndef CONVERSATION_H
#define CONVERSATION_H

#include "AppLogic/Entities/Message.h"
#include "AppLogic/Entities/User.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct Conversation {
    char* id;
    User** participants;
    size_t participant_count;
    size_t participant_capacity;
    Message** messages;
    size_t message_count;
    size_t message_capacity;
} Conversation;

Conversation* conversation_create(const char* id);
void conversation_destroy(Conversation* conv);

/* Takes ownership of the User pointer. */
bool conversation_add_participant(Conversation* conv, User* user);

/* Takes ownership of the Message pointer. */
bool conversation_add_message(Conversation* conv, Message* msg);

/* Borrowed pointers — do not free. */
const Message* conversation_find_message(const Conversation* conv, const char* msg_id);
const User*    conversation_find_participant(const Conversation* conv, const char* user_id);

#endif
