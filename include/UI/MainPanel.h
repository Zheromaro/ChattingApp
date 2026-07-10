#ifndef MAIN_PANEL_H
#define MAIN_PANEL_H

typedef struct TextBox TextBox ;
typedef struct Conversation Conversation ;
typedef struct User User ;
typedef struct UI_Event UI_Event;

void MainPanel(TextBox* tb, Conversation* conv, User* me, UI_Event* event);

#endif
