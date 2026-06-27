#include <clay.h>
#include <string.h>
#include "AppLogic/MainPanel.h"

// -----------------------------------------------------------------------------
// 1. STYLE SYSTEM (Telegram palette)
// -----------------------------------------------------------------------------
static const Clay_Color C_WHITE         = {255, 255, 255, 255};
static const Clay_Color C_BLACK         = {0, 0, 0, 255};
static const Clay_Color C_TELEGRAM_BLUE = {44, 147, 227, 255};
static const Clay_Color C_BG_GREY       = {231, 235, 240, 255};
static const Clay_Color C_INPUT_BG      = {244, 244, 245, 255};
static const Clay_Color C_PLACEHOLDER   = {142, 142, 147, 255};

static const Clay_CornerRadius RADIUS_INCOMING = {12, 12, 4, 12};
static const Clay_CornerRadius RADIUS_OUTGOING = {12, 12, 12, 4};
static const Clay_CornerRadius RADIUS_PILL     = {18, 18, 18, 18};

// -----------------------------------------------------------------------------
// 2. DATA MODEL
// -----------------------------------------------------------------------------
typedef enum {
    MSG_DIR_INCOMING,
    MSG_DIR_OUTGOING
} MsgDirection;

typedef struct {
    MsgDirection dir;
    const char *text;
    // Future: uint64_t id, timestamp, delivery_status, etc.
} Message;

// Demo conversation (replace with your dynamic array later)
static Message s_messages[] = {
    {MSG_DIR_INCOMING, "Hey there! How is the new UI engine holding up?"},
    {MSG_DIR_OUTGOING, "It's amazing! The layouts compile in microseconds."},
    {MSG_DIR_INCOMING, "Awesome! Send over a snippet when you get the text engine rendering properly."},
    {MSG_DIR_INCOMING, "Also, does it handle text wrapping automatically?"},
    {MSG_DIR_OUTGOING, "Yeah — as long as the parent has a constrained width, Clay wraps by word. You just provide the measure function."},
    {MSG_DIR_OUTGOING, "The only thing you need is a stable ID for transitions to work correctly."},
};
static int s_msgCount = sizeof(s_messages) / sizeof(s_messages[0]);

// Helper: build a Clay_String from runtime char* (CLAY_STRING only works with literals)
#define CLAY_STR(cstr) \
    (Clay_String){ .length = (int32_t)strlen(cstr), .chars = (cstr), .isStaticallyAllocated = true }

// -----------------------------------------------------------------------------
// 3. COMPONENTS
// -----------------------------------------------------------------------------

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
        CLAY_TEXT(CLAY_STRING("Alex"), {.fontSize = 18, .textColor = C_BLACK});
    }
}

static void MessageBubble(Message *msg, int index) {
    bool outgoing = (msg->dir == MSG_DIR_OUTGOING);

    Clay_Color bubbleColor = outgoing ? C_TELEGRAM_BLUE : C_WHITE;
    Clay_Color textColor   = outgoing ? C_WHITE        : C_BLACK;
    Clay_CornerRadius radius = outgoing ? RADIUS_OUTGOING : RADIUS_INCOMING;

    // Row spans full width. For outgoing messages, a flex spacer pushes the bubble right.
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

        // Bubble: fits text but caps at 500px so wrapping feels like Telegram.
        // Clay automatically wraps CLAY_TEXT inside a width-constrained parent.
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
            CLAY_TEXT(CLAY_STR(msg->text), {
                .fontSize = 15,
                .textColor = textColor,
                .wrapMode = CLAY_TEXT_WRAP_WORDS
            });
        }
    }
}

static void InputBar(void) {
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
        // Input capsule
        CLAY(CLAY_ID("InputCapsule"), {
            .layout = {
                .padding = CLAY_PADDING_ALL(10),
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
            },
            .backgroundColor = C_INPUT_BG,
            .cornerRadius = RADIUS_PILL
        }) {
            CLAY_TEXT(CLAY_STRING("Write a message..."), {.fontSize = 15, .textColor = C_PLACEHOLDER});
        }

        // Send button
        CLAY(CLAY_ID("SendButton"), {
            .layout = {
                .padding = {.left = 16, .right = 16, .top = 10, .bottom = 10},
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_GROW(0)},
                .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
            },
            .backgroundColor = C_TELEGRAM_BLUE,
            .cornerRadius = RADIUS_PILL
        }) {
            CLAY_TEXT(CLAY_STRING("Send"), {.fontSize = 15, .textColor = C_WHITE});
        }
    }
}

// -----------------------------------------------------------------------------
// 4. MAIN PANEL ASSEMBLY
// -----------------------------------------------------------------------------

void MainPanel(void) {
    CLAY(CLAY_ID("ChatWindow"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}
        }
    }) {
        ChatHeader();

        // Scrollable message stream
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
                MessageBubble(&s_messages[i], i);
            }
        }

        InputBar();
    }
}
