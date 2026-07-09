#include <stdio.h>
#include "UI/Widgets/Badge.h"
#include "UI/UI_Theme.h"

void UnreadBadge(int count)
{
    if (count <= 0) return;

    char buf[8];
    snprintf(buf, sizeof(buf), "%d", count > 99 ? 99 : count);

    CLAY(CLAY_ID_LOCAL("UnreadBadge"), {
        .layout = {
            .sizing = {.width = CLAY_SIZING_FIXED(20), .height = CLAY_SIZING_FIXED(20)},
            .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = (Clay_Color){255, 59, 48, 255},
        .cornerRadius = CLAY_CORNER_RADIUS(10)
    }) {
        CLAY_TEXT(CLAY_STR(buf), {
            .fontId   = FONT_ID_BODY_13,
            .fontSize = 11,
            .textColor = C_WHITE
        });
    }
}
