#include <string.h>
#include <clay.h>
#include "UI/UI_Theme.h"
#include "UI/Logic/TextBox.h"
#include "UI/Widgets/TextBox.h"

void TextInputContent(TextBox* tb, const char* placeholder,
                      uint16_t fontSize, Clay_Color textColor,
                      Clay_Color placeholderColor, uint16_t fontId)
{
    char*  full      = TBGetText(tb);
    bool   is_empty  = (!full || full[0] == '\0');
    size_t count     = TBGetCharCount(tb);
    size_t cursor    = TBGetCursorPos(tb);
    bool   has_sel   = TBHasSelection(tb);
    size_t sel_start = TBGetSelectionStart(tb);
    size_t sel_end   = TBGetSelectionEnd(tb);

    if (is_empty && !has_sel) {
        CLAY_TEXT(CLAY_STR(placeholder), {
            .fontId    = fontId,
            .fontSize  = fontSize,
            .textColor = placeholderColor
        });
        return;
    }

    size_t full_len = strlen(full);

    if (has_sel) {
        size_t sel_start_b = TBGetByteOffset(tb, sel_start);
        size_t sel_end_b   = TBGetByteOffset(tb, sel_end);

        /* before selection */
        if (sel_start > 0) {
            Clay_String s = {.chars = full, .length = (int32_t)sel_start_b};
            CLAY_TEXT(s, {.fontId = fontId, .fontSize = fontSize, .textColor = textColor});
        }

        /* selected slice */
        if (sel_end > sel_start) {
            Clay_String s = {
                .chars  = full + sel_start_b,
                .length = (int32_t)(sel_end_b - sel_start_b)
            };
            CLAY(CLAY_ID("SelBg"), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_GROW(0)},
                    .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
                },
                .backgroundColor = C_BLUE,
                .cornerRadius = {4, 4, 4, 4}
            }) {
                CLAY_TEXT(s, {.fontId = fontId, .fontSize = fontSize, .textColor = C_WHITE});
            }
        }

        /* after selection */
        if (sel_end < count) {
            Clay_String s = {
                .chars  = full + sel_end_b,
                .length = (int32_t)(full_len - sel_end_b)
            };
            CLAY_TEXT(s, {.fontId = fontId, .fontSize = fontSize, .textColor = textColor});
        }
    } else {
        /* cursor mode */
        size_t cursor_b = TBGetByteOffset(tb, cursor);

        if (cursor > 0) {
            Clay_String s = {.chars = full, .length = (int32_t)cursor_b};
            CLAY_TEXT(s, {.fontId = fontId, .fontSize = fontSize, .textColor = textColor});
        }

        Clay_Color cursorColor = TBIsCursorVisible(tb) ? textColor : (Clay_Color){0,0,0,0};
        CLAY(CLAY_ID("Cursor"), {
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIXED(2), .height = CLAY_SIZING_FIXED(18)},
                .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
            },
            .backgroundColor = cursorColor
        }) {}

        if (cursor < count) {
            Clay_String s = {
                .chars  = full + cursor_b,
                .length = (int32_t)(full_len - cursor_b)
            };
            CLAY_TEXT(s, {.fontId = fontId, .fontSize = fontSize, .textColor = textColor});
        }
    }
}
