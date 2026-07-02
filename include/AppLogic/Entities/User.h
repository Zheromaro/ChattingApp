#ifndef USER_H
#define USER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct User {
    char* id;
    char* display_name;
    char* avatar_url;
} User;

User* UserCreate(const char* id, const char* display_name, const char* avatar_url);
User* UserClone(const User* other);
void UserDestroy(User* user);

#ifdef __cplusplus
}
#endif

#endif
