#include "AppLogic/Entities/User.h"
#include <stdlib.h>
#include <string.h>

static char* str_dup(const char* s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char* d = malloc(n);
    if (d) memcpy(d, s, n);
    return d;
}

User* UserCreate(const char* id, const char* display_name, const char* avatar_url) {
    User* user = calloc(1, sizeof(User));
    if (!user) return NULL;

    user->id           = str_dup(id);
    user->display_name = str_dup(display_name);
    user->avatar_url   = str_dup(avatar_url);

    if ((id && !user->id) || (display_name && !user->display_name)) {
        UserDestroy(user);
        return NULL;
    }
    return user;
}

User* UserClone(const User* other) {
    if (!other) return NULL;
    return UserCreate(other->id, other->display_name, other->avatar_url);
}

void UserDestroy(User* user) {
    if (!user) return;
    free(user->id);
    free(user->display_name);
    free(user->avatar_url);
    free(user);
}
