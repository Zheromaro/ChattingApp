#include <clay.h>
#include <stdio.h>
#include "Core/Text.h"
#include "UI/TextBox.h"
#include "Model/Conversation.h"
#include "Model/User.h"
#include "Model/Message.h"
#include "UI/MainPanel.h"
#include "UI/ChatInputBar.h"
#include "UI/CONST_UI.h"
#include "UI/TextBox.h"

static void MessageBubble(const Message* msg, User* me, int index) {
    bool outgoing = strcmp(MessageGetAuthorID(msg), UserGetID(me)) == 0;

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
            CLAY_TEXT(CLAY_STR(MessageGetText(msg)), {
                .fontId = FONT_ID_BODY_15,
                .fontSize = 15,
                .textColor = textColor,
                .wrapMode = CLAY_TEXT_WRAP_WORDS
            });
        }
    }
}

static void MessageStream(Conversation* conv, User* me) {
    int msgCount = ConvGetMessageCount(conv);
    CLAY(CLAY_ID("MessageStream"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .padding = CLAY_PADDING_ALL(20),
            .childGap = 10,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}
        },
        .clip = {.vertical = true, .childOffset = Clay_GetScrollOffset()}
    }) {
        for (int i = 0; i < msgCount; i++) {
            MessageBubble(ConvGetMessage(conv, i), me, i);
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

void MainPanel(TextBox* tb, Conversation* conv, User* me) {
    CLAY(CLAY_ID("ChatWindow"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}
        }
    }) {
        ChatHeader();
        MessageStream(conv, me);
        ChatInputBar(tb);
    }
}
