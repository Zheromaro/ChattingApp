#ifndef USER_H
#define USER_H

typedef struct User User;

User* UserCreate(const char* display_name);
void UserDestroy(User* user);
const char* UserGetID(const User* user);
const char* UserGetName(const User* user);

#endif
