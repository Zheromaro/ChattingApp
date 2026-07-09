#include "UI/Widgets/ChatHeader.h"
#include "UI/UI_Theme.h"

void ChatHeader(const char* name)
{
    CLAY(CLAY_ID("ChatHeader"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .padding = CLAY_PADDING_ALL(16),
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(60)},
            .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = C_WHITE
    }) {
        CLAY_TEXT(CLAY_STR(name), {
            .fontId = FONT_ID_TITLE_18,
            .fontSize = 18,
            .textColor = C_BLACK
        });
    }
}
