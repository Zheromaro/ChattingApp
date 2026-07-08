#include <clay.h>
#include <string.h>
#include "Core/Text.h"
#include "UI/TextBox.h"
#include "UI/CONST_UI.h"
#include "UI/ChatInputBar.h"


static void SendButton(void) {
    CLAY(CLAY_ID("SendButton"), {
        .layout = {
            .padding = {.left = 16, .right = 16, .top = 10, .bottom = 10},
            .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_GROW(0)},
            .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = C_BLUE,
        .cornerRadius = RADIUS_PILL
    }) {
        CLAY_TEXT(CLAY_STRING("Send"), {.fontId = FONT_ID_BODY_15, .fontSize = 15, .textColor = C_WHITE});
    }
}

static void TextField(TextBox* tb) {
    char*  full      = TBGetText(tb);
    bool   is_empty  = (!full || full[0] == '\0');
    size_t count     = TBGetCharCount(tb);
    size_t cursor    = TBGetCursorPos(tb);
    bool   has_sel   = TBHasSelection(tb);
    size_t sel_start = TBGetSelectionStart(tb);
    size_t sel_end   = TBGetSelectionEnd(tb);

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
        // if empty
        if (is_empty && !has_sel) {
            CLAY_TEXT(CLAY_STRING("Type a message..."), {
                .fontId   = FONT_ID_BODY_15,
                .fontSize = 15,
                .textColor = C_PLACEHOLDER
            });
        } else {
            size_t full_len = strlen(full);

            // selection mode
            if (has_sel) {
                size_t sel_start_b = TBGetByteOffset(tb, sel_start);
                size_t sel_end_b   = TBGetByteOffset(tb, sel_end);

                /* text before selection */
                if (sel_start > 0) {
                    Clay_String s = {.chars = full, .length = sel_start_b};
                    CLAY_TEXT(s, {
                        .fontId   = FONT_ID_BODY_15,
                        .fontSize = 15,
                        .textColor = C_BLACK
                    });
                }

                /* selected slice with highlight background */
                if (sel_end > sel_start) {
                    Clay_String s = {
                        .chars   = full + sel_start_b,
                        .length  = sel_end_b - sel_start_b
                    };
                    CLAY(CLAY_ID("SelBg"), {
                        .layout = {
                            .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_GROW(0)},
                            .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
                        },
                        .backgroundColor = C_BLUE,          /* or a dedicated C_SELECTION_BG */
                        .cornerRadius = {4, 4, 4, 4}
                    }) {
                        CLAY_TEXT(s, {
                            .fontId   = FONT_ID_BODY_15,
                            .fontSize = 15,
                            .textColor = C_WHITE
                        });
                    }
                }

                /* text after selection */
                if (sel_end < count) {
                    Clay_String s = {
                        .chars   = full + sel_end_b,
                        .length  = full_len - sel_end_b
                    };
                    CLAY_TEXT(s, {
                        .fontId   = FONT_ID_BODY_15,
                        .fontSize = 15,
                        .textColor = C_BLACK
                    });
                }
            }
            // cursor mode (no selection)
            else {
                size_t cursor_b = TBGetByteOffset(tb, cursor);

                /* text before caret */
                if (cursor > 0) {
                    Clay_String s = {.chars = full, .length = cursor_b};
                    CLAY_TEXT(s, {
                        .fontId   = FONT_ID_BODY_15,
                        .fontSize = 15,
                        .textColor = C_BLACK
                    });
                }

                /* blinking caret */
                Clay_Color CursorColor = TBIsCursorVisible(tb)? C_BLACK : C_WHITE;
                CLAY(CLAY_ID("Cursor"), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_FIXED(2), .height = CLAY_SIZING_FIXED(18)},
                        .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
                    },
                    .backgroundColor = CursorColor
                }) {}

                /* text after caret */
                if (cursor < count) {
                    Clay_String s = {
                        .chars   = full + cursor_b,
                        .length  = full_len - cursor_b
                    };
                    CLAY_TEXT(s, {
                        .fontId   = FONT_ID_BODY_15,
                        .fontSize = 15,
                        .textColor = C_BLACK
                    });
                }
            }
        }
    }
}

void ChatInputBar(TextBox* tb) {
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
