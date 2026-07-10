#ifndef UI_EVENT_H
#define UI_EVENT_H

typedef void(*OnSendClicked)(char*);
typedef void(*OnSearchTexBoxClicked)(void);
typedef void(*OnChatTexBoxClicked)(void);

typedef struct UI_Event {
    OnSendClicked send;
    OnChatTexBoxClicked chatBox;
    OnSearchTexBoxClicked searchBox;
} UI_Event;

#endif
