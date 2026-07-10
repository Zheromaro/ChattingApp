#include <clay.h>
#include "Core/UI.h"
#include "UI/Logic/TextBox.h"
#include "UI/UI_Event.h"
#include "UI/SideBar.h"
#include "UI/Widgets/TextBox.h"
#include "UI/ChatInputBar.h"
#include "UI/UI_Theme.h"

static void SendButton(TextBox* tb, UI_Event* event)
{
    CLAY(CLAY_ID("SendButton"), {
        .layout = {
            .padding = {.left = 16, .right = 16, .top = 10, .bottom = 10},
            .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_GROW(0)},
            .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = C_BLUE,
        .cornerRadius = RADIUS_PILL
    }) {
        const bool isHovered = Clay_Hovered();
        if (isHovered && UI_GetMouseDown()) event->send(TBTakeText(tb));
        CLAY_TEXT(CLAY_STRING("Send"), {
            .fontId = FONT_ID_BODY_15,
            .fontSize = 15,
            .textColor = C_WHITE
        });
    }
}

static void TextField(TextBox* tb, UI_Event* event)
{
    CLAY(CLAY_ID("ChatInput"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .padding = CLAY_PADDING_ALL(10),
            .sizing  = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
            .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = C_INPUT_BG,
        .cornerRadius = RADIUS_PILL
    }) {
        const bool isHovered = Clay_Hovered();
        if (isHovered && UI_GetMouseDown()) event->chatBox();
        TextInputContent(tb, "Type a message...", 15, C_BLACK, C_PLACEHOLDER, FONT_ID_BODY_15);
    }
}

void ChatInputBar(TextBox* tb, UI_Event* event)
{
    CLAY(CLAY_ID("BottomInputBar"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .padding = CLAY_PADDING_ALL(12),
            .childGap = 12,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(64)},
            .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = C_WHITE
    }) {
        TextField(tb, event);
        SendButton(tb, event);
    }
}
