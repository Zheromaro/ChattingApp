#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "LoopLogic/UI.h"
#include "AppLogic/Chat.h"
#include "AppLogic/UI/SideBar.h"
#include "AppLogic/UI/MainPanel.h"
#include <SDL3/SDL_oldnames.h>
#include <string.h>
#include <wchar.h>

char* messageBuffer = '\0';
int messageBuffLen = 0;
int addLen = 0;
int oldLen = 0;

void ChatInput(SDL_Event* event) {
    switch (event->type) {
        case SDL_EVENT_TEXT_INPUT:
            addLen = strlen(event->text.text);
            oldLen = messageBuffLen;
            messageBuffLen += addLen;

            char* newMessage = realloc(messageBuffer, messageBuffLen + 1); // +1 for '\0'
            if (newMessage == NULL) {
                fprintf(stderr, "Error: Failed realloc for text input\n");
                break;
            }

            strcpy(newMessage + oldLen, event->text.text); // append at OLD length
            newMessage[messageBuffLen] = '\0';
            messageBuffer = newMessage;
            break;
    }
}

void ChatUpdate(float delta_time) {
    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
        },
        .backgroundColor = { 231, 235, 240, 255 }
    }) {
        SideBar();
        MainPanel(messageBuffer);
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout(delta_time);
    UI_Layout(renderCommands);
}

void ChatExit(void) {
    free(messageBuffer);
}
