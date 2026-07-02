#ifndef MESSAGE_LIST_H
#define MESSAGE_LIST_H

#include <stddef.h>

typedef struct Message Message;
typedef struct MessageManager MessageManager;

void MMInit(MessageManager* mm);
void MMFree(MessageManager* mm);
void MMAdd(MessageManager* mm, const char* author_id, const char* conversation_id, const char* text);

size_t MMCount(const MessageManager* mm);

#endif
