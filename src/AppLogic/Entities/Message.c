#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "AppLogic/Entities/Message.h"
#include "AppLogic/ID.h"

typedef struct Message {
    char* id;
    char* author_id;
    char* conversation_id;
    char* text;
    struct tm timestamp;
} Message;

static char* str_dup(const char* s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char* d = malloc(n);
    if (d) memcpy(d, s, n);
    return d;
}


Message* MessageCreate(const char* author_id, const char* conversation_id, const char* text) {
    Message* msg = calloc(1, sizeof(Message));
    if (!msg) return NULL;

    time_t now = time(NULL);
    struct tm *tm_ptr = gmtime(&now);
    if (!tm_ptr) { MessageDestroy(msg); return NULL; }

    msg->id            = GenerateIDString();
    msg->author_id     = str_dup(author_id);
    msg->conversation_id = str_dup(conversation_id);
    msg->text          = str_dup(text);
    msg->timestamp     = *tm_ptr;

    if (!msg->id ||(conversation_id && !msg->conversation_id) ||
        (author_id && !msg->author_id) || (text && !msg->text)) {
        MessageDestroy(msg);
        return NULL;
    }
    return msg;
}

const char* MessageGetID(const Message* msg) {
    return msg->id;
}

const char* MessageGetAuthorID(const Message* msg) {
    return msg->author_id;
}

const char* MessageGetText(const Message* msg) {
    return msg->text;
}

void MessageDestroy(Message* msg) {
    if (!msg) return;
    free(msg->id);
    free(msg->author_id);
    free(msg->conversation_id);
    free(msg->text);
    free(msg);
}
