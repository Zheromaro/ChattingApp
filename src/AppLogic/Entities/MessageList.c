#include "AppLogic/Entities/MessageList.h"
#include "AppLogic/Entities/Message.h"
#include <stdlib.h>
#include <string.h>

struct MessageManager {
    Message* messages;
    size_t count;
    size_t capacity;
};

void MMInit(MessageManager* mm) {
    mm->messages = NULL;
    mm->count = 0;
    mm->capacity = 0;
}

void MMFree(MessageManager* mm) {
    if (!mm) return;
    // for (size_t i = 0; i < mm->count; i++) free(mm->messages[i].text);
    free(mm->messages);
    mm->messages = NULL;
    mm->count = 0;
    mm->capacity = 0;
}

//void MMAdd(MessageManager* mm, MsgSource source, const char* text) {
void MMAdd(MessageManager* mm, const char* author_id, const char* conversation_id, const char* text) {
    if (!mm || !text || !*text) return;
    if (mm->count >= mm->capacity) {
        size_t new_cap = mm->capacity ? mm->capacity * 2 : 8;
        Message* tmp = realloc(mm->messages, 1);//new_cap * sizeof(Message));
        if (!tmp) return;
        mm->messages = tmp;
        mm->capacity = new_cap;
    }
    //mm->messages[mm->count].source = source;
    //mm->messages[mm->count].text = string_dupstr(text);
    mm->count++;
}

const Message* MMGet(const MessageManager* mm, size_t index) {
    //return (mm && index < mm->count) ? &mm->messages[index] : NULL;
}

size_t MMCount(const MessageManager* mm) {
    return mm ? mm->count : 0;
}
