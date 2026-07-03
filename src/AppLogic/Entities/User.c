#include <stdlib.h>
#include <string.h>
#include "AppLogic/Entities/User.h"
#include "AppLogic/ID.h"


typedef struct User {
    char* id;
    char* display_name;
} User;

static char* str_dup(const char* s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char* d = malloc(n);
    if (d) memcpy(d, s, n);
    return d;
}

User* UserCreate(const char* display_name) {
    User* user = calloc(1, sizeof(User));
    if (!user) return NULL;

    user->id           = GenerateIDString();
    user->display_name = str_dup(display_name);

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
    free(user);
}

const char* UserGetID(const User* user) {
    return user ? user->id : NULL;
}

const char* UserGetName(const User* user) {
    return user ? user->display_name : NULL;
}
