#ifndef UI_SIDEBAR_H
#define UI_SIDEBAR_H

typedef struct Conversation Conversation;
typedef struct TextBox TextBox;

void SideBar(Conversation **conversations, int count, int *active_index, const TextBox *search_tb);

#endif
