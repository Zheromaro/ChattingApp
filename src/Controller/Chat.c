#include "Core/UI.h"
#include "Controller/Chat.h"
#include "UI/SideBar.h"
#include "UI/MainPanel.h"
#include "UI/TextBox.h"
#include "Model/Message.h"
#include "Model/User.h"
#include "Model/Conversation.h"
#include <stdio.h>

TextBox *tb = NULL;
Message* msg;
User* me;
Conversation* conv;

void ChatEnter(void) {
    tb = TBCreate();
    me = UserCreate("zahrawi");
    conv = ConvCreate();
    ConvAddParticipant(conv, me);
}

void ChatExit(void) {
    TBDestroy(tb);
    UserDestroy(me);
    ConvDestroy(conv);
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
                    if (messageBuffer[0] == '\0') break;

                    msg = MessageCreate(UserGetID(me), ConvGetID(conv), messageBuffer);
                    ConvAddMessage(conv, msg);
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
        MainPanel(tb, conv, me);
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout(delta_time);
    UI_Layout(renderCommands);
}

void ChatRender(SDL_Renderer *renderer) {
    (void)renderer;
}
