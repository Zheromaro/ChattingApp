#ifndef UI_SIDEBAR_H
#define UI_SIDEBAR_H

typedef struct ContactList ContactList;
typedef struct TextBox TextBox;
typedef struct UI_Event UI_Event;

void SideBar(ContactList *cl, const TextBox *search_tb, UI_Event* event);

#endif
