#ifndef CONTACT_LIST_H
#define CONTACT_LIST_H

#include <stdbool.h>

typedef struct ContactList ContactList;
typedef struct Conversation Conversation;

ContactList* CLCreate(void);
void CLDestroy(ContactList* cl);

/* --- Modifiers --- */
bool CLAddConversation(ContactList* cl, Conversation* conv);
void CLSetActiveIndex(ContactList* cl, int index);

/* --- Getters --- */
int CLGetCount(const ContactList* cl);
int CLGetActiveIndex(const ContactList* cl);
Conversation* CLGetConversation(const ContactList* cl, int index);
Conversation* CLGetActiveConversation(const ContactList* cl);
Conversation** CLGetConversationsArray(const ContactList* cl);

#endif // CONTACT_LIST_H
