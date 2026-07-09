#include <clay.h>
#include "Core/Text.h"
#include "UI/Widgets/TextBox.h"
#include "UI/ChatInputBar.h"
#include "UI/UI_Theme.h"

static void SendButton(void)
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
        CLAY_TEXT(CLAY_STRING("Send"), {
            .fontId = FONT_ID_BODY_15,
            .fontSize = 15,
            .textColor = C_WHITE
        });
    }
}

static void TextField(TextBox* tb)
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
        TextInputContent(tb, "Type a message...", 15, C_BLACK, C_PLACEHOLDER, FONT_ID_BODY_15);
    }
}

void ChatInputBar(TextBox* tb)
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
        TextField(tb);
        SendButton();
    }
}
