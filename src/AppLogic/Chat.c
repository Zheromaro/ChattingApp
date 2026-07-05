#include "AppLogic/Entities/Message.h"
#include "LoopLogic/UI.h"
#include "AppLogic/Chat.h"
#include "AppLogic/UI/SideBar.h"
#include "AppLogic/UI/MainPanel.h"
#include "AppLogic/Entities/TextBox.h"
#include <stdio.h>

TextBox *tb = NULL;
Message* msg;

void ChatEnter(void) {
    tb = TBCreate();
}

void ChatExit(void) {
    TBDestroy(tb);
}

void ChatInput(SDL_Event* event) {
    if (TBHandleEvent(tb, event)) {
    }

    switch (event->type) {
        case SDL_EVENT_KEY_DOWN:
            switch (event->key.key) {
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    const char* messageBuffer = TBTakeText(tb);
                    if (messageBuffer[0] != '\0') {
                        msg = MessageCreate("me", "us", messageBuffer);
                    }
                    printf("%s\n", MessageGetText(msg));
                    break;
            }
            break;
    }
}

void ChatUpdate(float delta_time) {
    TBUpdate(tb, delta_time);
    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
        },
        .backgroundColor = { 231, 235, 240, 255 }
    }) {
        SideBar();
        MainPanel(tb);
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout(delta_time);
    UI_Layout(renderCommands);
}

void ChatRender(SDL_Renderer *renderer) {
    (void)renderer;
}
