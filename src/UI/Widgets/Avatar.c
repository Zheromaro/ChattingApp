#include "UI/Widgets/Avatar.h"
#include "UI/UI_Theme.h"

void Avatar(int index, const char* name, bool isActive, bool isHovered)
{
    const Clay_Color bg = isActive ? C_WHITE : CLAY_HASH_COLOR(index * 20);
    const float size    = (isHovered && !isActive) ? AVATAR_SIZE_HOVERED : AVATAR_SIZE_DEFAULT;
    const char initial  = (name && name[0]) ? name[0] : '?';

    CLAY(CLAY_IDI("Avatar", index), {
        .layout = {
            .sizing = {.width = CLAY_SIZING_FIXED(size), .height = CLAY_SIZING_FIXED(size)},
            .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = bg,
        .cornerRadius = CLAY_CORNER_RADIUS(size / 2.0f)
    }) {
        const char str[2] = { initial, '\0' };
        CLAY_TEXT(CLAY_STR(str), {
            .fontId   = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = isActive ? CLAY_HASH_COLOR(index * 20) : C_WHITE
        });
    }
}
