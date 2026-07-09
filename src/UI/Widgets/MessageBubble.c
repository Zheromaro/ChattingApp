#include <string.h>
#include <clay.h>
#include "UI/Widgets/MessageBubble.h"
#include "UI/UI_Theme.h"


void MessageBubble(int index, const Message* msg, const User* me)
{
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
