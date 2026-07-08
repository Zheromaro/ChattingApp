#include <stdlib.h>
#include <string.h>
#include "Model/Conversation.h"
#include "Model/Message.h"
#include "Model/User.h"
#include "Helper/ID.h"

struct Conversation {
    char* id;
    User** participants;
    size_t participant_count;
    size_t participant_capacity;
    Message** messages;
    size_t message_count;
    size_t message_capacity;
};

static bool grow_array(void** arr, size_t* capacity, size_t count, size_t item_size) {
    if (count < *capacity) return true;
    size_t new_cap = *capacity == 0 ? 4 : *capacity * 2;
    void* new_arr = realloc(*arr, new_cap * item_size);
    if (!new_arr) return false;
    *arr = new_arr;
    *capacity = new_cap;
    return true;
}

Conversation* ConvCreate(void) {
    Conversation* conv = calloc(1, sizeof(Conversation));
    if (!conv) return NULL;
    conv->id = GenerateIDString();
    if (!conv->id) {
        free(conv);
        return NULL;
    }
    return conv;
}

void ConvDestroy(Conversation* conv) {
    if (!conv) return;
    for (size_t i = 0; i < conv->participant_count; i++) UserDestroy(conv->participants[i]);
    free(conv->participants);
    for (size_t i = 0; i < conv->message_count; i++) MessageDestroy(conv->messages[i]);
    free(conv->messages);
    free(conv->id);
    free(conv);
}

bool ConvAddParticipant(Conversation* conv, User* user) {
    if (!conv || !user) return false;
    if (!grow_array((void**)&conv->participants, &conv->participant_capacity,
                    conv->participant_count, sizeof(User*))) {
        return false;
    }
    conv->participants[conv->participant_count++] = user;
    return true;
}

bool ConvAddMessage(Conversation* conv, Message* msg) {
    if (!conv || !msg) return false;
    if (!grow_array((void**)&conv->messages, &conv->message_capacity,
                    conv->message_count, sizeof(Message*))) {
        return false;
    }
    conv->messages[conv->message_count++] = msg;
    return true;
}

const User* ConvFindParticipant(const Conversation* conv, const char* user_id) {
    if (!conv || !user_id) return NULL;
    for (size_t i = 0; i < conv->participant_count; i++) {
        if (strcmp(UserGetID(conv->participants[i]), user_id) == 0)
            return conv->participants[i];
    }
    return NULL;
}

const char* ConvGetID(const Conversation* conv) {
    return conv ? conv->id : NULL;
}

size_t ConvGetMessageCount(const Conversation* conv) {
    return conv ? conv->message_count : 0;
}

const Message* ConvGetMessage(const Conversation* conv, size_t index) {
    if (!conv || index >= conv->message_count) return NULL;
    return conv->messages[index];
}
