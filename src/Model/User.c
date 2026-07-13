#include <stdlib.h>
#include "Model/User.h"
#include "Helper/ID.h"
#include "Helper/HStrings.h"

typedef struct User {
    char* id;
    char* display_name;
    char* avatarColor;
    char* peer_addr;
} User;

User* UserCreate(const char* display_name) {
    User* user = calloc(1, sizeof(User));
    if (!user) return NULL;

    user->id           = GenerateIDString();
    user->display_name = string_dup(display_name);
    user->peer_addr    = NULL;

    if ((!user->id) || (display_name && !user->display_name)) {
        UserDestroy(user);
        return NULL;
    }
    return user;
}

void UserDestroy(User* user) {
    if (!user) return;
    free(user->id);
    free(user->display_name);
    free(user->avatarColor);
    free(user->peer_addr);
    free(user);
}

const char* UserGetID(const User* user) {
    return user ? user->id : NULL;
}

const char* UserGetName(const User* user) {
    return user ? user->display_name : NULL;
}

void UserSetPeerAddr(User* user, const char* addr) {
    if (!user) return;
    free(user->peer_addr);
    user->peer_addr = addr ? strdup(addr) : NULL;
}

const char* UserGetPeerAddr(const User* user) {
    return user ? user->peer_addr : NULL;
}
