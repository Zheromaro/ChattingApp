#include "AppLogic/Entities/Conversation.h"
#include <stdlib.h>
#include <string.h>

static char* str_dup(const char* s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char* d = malloc(n);
    if (d) memcpy(d, s, n);
    return d;
}

static bool grow_array(void** arr, size_t* capacity, size_t count, size_t item_size) {
    if (count < *capacity) return true;
    size_t new_cap = *capacity == 0 ? 4 : *capacity * 2;
    void* new_arr = realloc(*arr, new_cap * item_size);
    if (!new_arr) return false;
    *arr = new_arr;
    *capacity = new_cap;
    return true;
}

Conversation* conversation_create(const char* id) {
    Conversation* conv = calloc(1, sizeof(Conversation));
    if (!conv) return NULL;
    conv->id = str_dup(id);
    if (id && !conv->id) {
        free(conv);
        return NULL;
    }
    return conv;
}

void conversation_destroy(Conversation* conv) {
    if (!conv) return;
    for (size_t i = 0; i < conv->participant_count; i++) UserDestroy(conv->participants[i]);
    free(conv->participants);
    for (size_t i = 0; i < conv->message_count; i++) MessageDestroy(conv->messages[i]);
    free(conv->messages);
    free(conv->id);
    free(conv);
}

bool conversation_add_participant(Conversation* conv, User* user) {
    if (!conv || !user) return false;
    if (!grow_array((void**)&conv->participants, &conv->participant_capacity,
                    conv->participant_count, sizeof(User*))) {
        return false;
    }
    conv->participants[conv->participant_count++] = user;
    return true;
}

bool conversation_add_message(Conversation* conv, Message* msg) {
    if (!conv || !msg) return false;
    if (!grow_array((void**)&conv->messages, &conv->message_capacity,
                    conv->message_count, sizeof(Message*))) {
        return false;
    }
    conv->messages[conv->message_count++] = msg;
    return true;
}

const Message* conversation_find_message(const Conversation* conv, const char* msg_id) {
    if (!conv || !msg_id) return NULL;
    for (size_t i = 0; i < conv->message_count; i++) {
        //if (conv->messages[i]->id && strcmp(conv->messages[i]->id, msg_id) == 0)
        //    return conv->messages[i];
    }
    return NULL;
}

const User* conversation_find_participant(const Conversation* conv, const char* user_id) {
    if (!conv || !user_id) return NULL;
    for (size_t i = 0; i < conv->participant_count; i++) {
        if (conv->participants[i]->id && strcmp(conv->participants[i]->id, user_id) == 0)
            return conv->participants[i];
    }
    return NULL;
}
