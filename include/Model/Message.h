#ifndef MESSAGE_H
#define MESSAGE_H

typedef struct Message Message;

Message* MessageCreate(const char* author_id, const char* conversation_id, const char* text);
const char* MessageGetID(const Message* msg);
const char* MessageGetAuthorID(const Message* msg);
const char* MessageGetText(const Message* msg);
void MessageDestroy(Message* msg);

#endif
