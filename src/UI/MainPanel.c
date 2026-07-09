#include <clay.h>
#include "UI/MainPanel.h"
#include "UI/ChatInputBar.h"
#include "UI/Widgets/MessageBubble.h"
#include "UI/Widgets/ChatHeader.h"
#include "Model/Conversation.h"
#include "Model/User.h"

static const char* GetConversationTitle(const Conversation* conv)
{
    if (!conv) return "Chat";
    const User* u = ConvGetParticipant(conv, 0);
    return (u && UserGetName(u)) ? UserGetName(u) : "Chat";
}

static void MessageStream(const Conversation* conv, const User* me)
{
    size_t msgCount = ConvGetMessageCount(conv);
    CLAY(CLAY_ID("MessageStream"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .padding = CLAY_PADDING_ALL(20),
            .childGap = 10,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}
        },
        .clip = {.vertical = true, .childOffset = Clay_GetScrollOffset()}
    }) {
        for (size_t i = 0; i < msgCount; i++) {
            MessageBubble((int)i, ConvGetMessage(conv, i), me);
        }
    }
}

void MainPanel(TextBox* tb, Conversation* conv, User* me)
{
    CLAY(CLAY_ID("ChatWindow"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}
        }
    }) {
        ChatHeader(GetConversationTitle(conv));
        MessageStream(conv, me);
        ChatInputBar(tb);
    }
}
