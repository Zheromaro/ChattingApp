#include <stdlib.h>
#include "Model/User.h"
#include "Helper/ID.h"
#include "Helper/HStrings.h"

typedef struct User {
    char* id;
    char* display_name;
    char* avatarColor;
} User;

User* UserCreate(const char* display_name) {
    User* user = calloc(1, sizeof(User));
    if (!user) return NULL;

    user->id           = GenerateIDString();
    user->display_name = string_dup(display_name);

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
    free(user);
}

const char* UserGetID(const User* user) {
    return user ? user->id : NULL;
}

const char* UserGetName(const User* user) {
    return user ? user->display_name : NULL;
}
