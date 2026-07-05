#include <clay.h>
#include "LoopLogic/Text.h"
#include "AppLogic/UI/MainPanel.h"
#include "AppLogic/UI/CONST_UI.h"
#include "AppLogic/Entities/TextBox.h"

// ===== DATA =====
typedef enum {
    INCOMING,
    OUTGOING
} MsgSource;

typedef struct {
    MsgSource source;
    const char *text;
} Message;

static Message s_messages[] = {
    {INCOMING, "Hey there! How is the new UI engine holding up?"},
    {OUTGOING, "It's amazing! The layouts compile in microseconds."},
    {INCOMING, "Awesome! Send over a snippet when you get the text engine rendering properly."},
    {INCOMING, "Also, does it handle text wrapping automatically?"},
    {OUTGOING, "Yeah — as long as the parent has a constrained width, Clay wraps by word. You just provide the measure function."},
    {OUTGOING, "The only thing you need is a stable ID for transitions to work correctly."},
};
static int s_msgCount = sizeof(s_messages) / sizeof(s_messages[0]);

// ===== UI =====
static void MessageBubble(Message msg, int index) {
    bool outgoing = (msg.source == OUTGOING);

    Clay_Color bubbleColor = outgoing ? C_BLUE  : C_WHITE;
    Clay_Color textColor   = outgoing ? C_WHITE : C_BLACK;
    Clay_CornerRadius radius = outgoing ? RADIUS_OUTGOING : RADIUS_INCOMING;

    CLAY(CLAY_IDI("MsgRow", index), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)}
        }
    }) {
        if (outgoing) {
            CLAY(CLAY_IDI("MsgSpacer", index), {
                .layout = {.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(1)}}
            }) {}
        }

        CLAY(CLAY_IDI("MsgBubble", index), {
            .layout = {
                .padding = CLAY_PADDING_ALL(12),
                .sizing = {
                    .width = CLAY_SIZING_FIT(.max = 500.0),
                    .height = CLAY_SIZING_FIT(0)
                }
            },
            .backgroundColor = bubbleColor,
            .cornerRadius = radius
        }) {
            // FIXED: Added FONT_ID_BODY_15
            CLAY_TEXT(CLAY_STR(msg.text), {
                .fontId = FONT_ID_BODY_15,
                .fontSize = 15,
                .textColor = textColor,
                .wrapMode = CLAY_TEXT_WRAP_WORDS
            });
        }
    }
}

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

static void ChatHeader(void) {
    CLAY(CLAY_ID("ChatHeader"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .padding = CLAY_PADDING_ALL(16),
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(60)},
            .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = C_WHITE
    }) {
        CLAY_TEXT(CLAY_STRING("Alex"), {.fontId = FONT_ID_TITLE_18, .fontSize = 18, .textColor = C_BLACK});
    }
}

static void MessageStream(void) {
    CLAY(CLAY_ID("MessageStream"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .padding = CLAY_PADDING_ALL(20),
            .childGap = 10,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}
        },
        .clip = {.vertical = true, .childOffset = Clay_GetScrollOffset()}
    }) {
        for (int i = 0; i < s_msgCount; i++) {
            MessageBubble(s_messages[i], i);
        }
    }
}

static void ChatInputBar(TextBox* tb) {
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

void MainPanel(TextBox* tb) {
    CLAY(CLAY_ID("ChatWindow"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}
        }
    }) {
        ChatHeader();
        MessageStream();
        ChatInputBar(tb);
    }
}
