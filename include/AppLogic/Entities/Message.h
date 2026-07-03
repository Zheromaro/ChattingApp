#ifndef MESSAGE_H
#define MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Message Message;

Message* MessageCreate(const char* author_id, const char* conversation_id, const char* text);
const char* MessageGetID(const Message* msg);
const char* MessageGetAuthorID(const Message* msg);
const char* MessageGetText(const Message* msg);
void MessageDestroy(Message* msg);

#ifdef __cplusplus
}
#endif

#endif
