#include "Model/ContactList.h"
#include "Model/Conversation.h"
#include <stdlib.h>

struct ContactList {
    Conversation** conversations;
    int convCount;
    int capacity;
    int activeIndex;
};

ContactList* CLCreate(void) {
    ContactList* cl = malloc(sizeof(ContactList));
    if (!cl) return NULL;

    cl->capacity = 10; // Start with a default capacity
    cl->conversations = malloc(cl->capacity * sizeof(Conversation*));
    if (!cl->conversations) {
        free(cl);
        return NULL;
    }

    cl->convCount = 0;
    cl->activeIndex = 0;

    return cl;
}

void CLDestroy(ContactList* cl) {
    if (!cl) return;

    for (int i = 0; i < cl->convCount; i++) {
        if (cl->conversations[i]) {
            ConvDestroy(cl->conversations[i]);
        }
    }

    free(cl->conversations);
    free(cl);
}

bool CLAddConversation(ContactList* cl, Conversation* conv) {
    if (!cl || !conv) return false;

    // Resize array if we hit capacity limit
    if (cl->convCount >= cl->capacity) {
        int newCapacity = cl->capacity * 2;
        Conversation** newArr = realloc(cl->conversations, newCapacity * sizeof(Conversation*));
        if (!newArr) return false;

        cl->conversations = newArr;
        cl->capacity = newCapacity;
    }

    cl->conversations[cl->convCount++] = conv;
    return true;
}

void CLSetActiveIndex(ContactList* cl, int index) {
    if (!cl) return;
    if (index >= 0 && index < cl->convCount) {
        cl->activeIndex = index;
    }
}

int CLGetCount(const ContactList* cl) {
    return cl ? cl->convCount : 0;
}

int CLGetActiveIndex(const ContactList* cl) {
    return cl ? cl->activeIndex : -1;
}

Conversation* CLGetConversation(const ContactList* cl, int index) {
    if (!cl || index < 0 || index >= cl->convCount) return NULL;
    return cl->conversations[index];
}

Conversation* CLGetActiveConversation(const ContactList* cl) {
    if (!cl || cl->activeIndex < 0 || cl->activeIndex >= cl->convCount) return NULL;
    return cl->conversations[cl->activeIndex];
}

Conversation** CLGetConversationsArray(const ContactList* cl) {
    return cl ? cl->conversations : NULL;
}
