#include <clay.h>
#include "AppLogic/UI/MainPanel.h"
#include "AppLogic/UI/CONST_UI.h"

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

static void SendButton() {
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

static void TextBox(char* messageBuffer) {
    CLAY(CLAY_ID("ChatInput"), {
        .layout = {
            .padding = CLAY_PADDING_ALL(10),
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
            .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = C_INPUT_BG,
        .cornerRadius = RADIUS_PILL
    }) {
        CLAY_TEXT(CLAY_STR(messageBuffer), {.fontId = FONT_ID_BODY_15, .fontSize = 15, .textColor = C_PLACEHOLDER});
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

static void MessageStream() {
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

static void ChatInputBar(char* messageBuffer) {
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
        TextBox(messageBuffer);
        SendButton();
    }
}

void MainPanel(char* messageBuffer) {
    CLAY(CLAY_ID("ChatWindow"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}
        }
    }) {
        ChatHeader();
        MessageStream();
        ChatInputBar(messageBuffer);
    }
}
