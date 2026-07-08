#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Core/UI.h"
#include "Controller/Chat.h"
#include "UI/SideBar.h"
#include "UI/MainPanel.h"
#include "UI/TextBox.h"
#include "UI/CONST_UI.h"
#include "Model/Message.h"
#include "Model/User.h"
#include "Model/Conversation.h"

/* ── State ──────────────────────────────────────────────────────── */

static TextBox* s_chat_tb = NULL;
static TextBox* s_search_tb = NULL;
static User*    s_me = NULL;

static Conversation** s_conversations = NULL;
static int            s_conv_count    = 0;
static int            s_active_conv   = 0;

/* ── Demo data ──────────────────────────────────────────────────── */

static void CreateDemoConversations(void) {
    static const char* names[] = {
        "Alex", "Sarah", "Mike", "Emma",
        "David", "Lisa", "Tom", "Jenny"
    };
    static const char* lastMessages[] = {
        "Typing dynamic layout rules...",
        "See you tomorrow!",
        "Can you review the PR?",
        "Thanks for the help!",
        "Meeting at 3pm",
        "Sent the files",
        "Sounds good to me",
        "Let me check..."
    };
    static const int unread[] = { 0, 2, 0, 5, 0, 1, 0, 3 };

    s_conv_count = 8;
    s_conversations = calloc((size_t)s_conv_count, sizeof(Conversation*));

    for (int i = 0; i < s_conv_count; i++) {
        s_conversations[i] = ConvCreate();
        if (!s_conversations[i]) continue;

        User* contact = UserCreate(names[i]);
        if (contact) {
            ConvAddParticipant(s_conversations[i], contact);
        }

        Message* msg = MessageCreate(
            UserGetID(contact),
            ConvGetID(s_conversations[i]),
            lastMessages[i]
        );
        if (msg) {
            ConvAddMessage(s_conversations[i], msg);
        }

        ConvSetUnreadCount(s_conversations[i], (size_t)unread[i]);
    }
}

/* ── Lifecycle ──────────────────────────────────────────────────── */

void ChatEnter(void)
{
    s_chat_tb = TBCreate();
    s_search_tb = TBCreate();
    s_me = UserCreate("zahrawi");
    CreateDemoConversations();
}

void ChatExit(void)
{
    TBDestroy(s_chat_tb);
    TBDestroy(s_search_tb);
    s_chat_tb = NULL;
    s_search_tb = NULL;

    UserDestroy(s_me);
    s_me = NULL;

    for (int i = 0; i < s_conv_count; i++) {
        if (s_conversations[i]) {
            ConvDestroy(s_conversations[i]);
        }
    }
    free(s_conversations);
    s_conversations = NULL;
    s_conv_count = 0;
}

/* ── Input handling ───────────────────────────────────────────────── */

void ChatInput(SDL_Event* event)
{
    if (!event) return;
    bool yes = true;

    if (yes) {
        TBHandleEvent(s_search_tb, event);
    } else if (yes) {
        TBHandleEvent(s_chat_tb, event);
    }

    /* Global shortcuts */
    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            case SDLK_RETURN:
            case SDLK_KP_ENTER: {
                const char* text = TBTakeText(s_chat_tb);
                if (!text || text[0] == '\0') break;
                if (s_active_conv < 0 || s_active_conv >= s_conv_count) break;
                if (!s_conversations[s_active_conv]) break;

                Message* msg = MessageCreate(
                    UserGetID(s_me),
                    ConvGetID(s_conversations[s_active_conv]),
                    text
                );
                if (msg) {
                    ConvAddMessage(s_conversations[s_active_conv], msg);
                }
                break;
            }
        }
    }
}

/* ── Update / Render ──────────────────────────────────────────────── */

void ChatUpdate(float delta_time)
{
    TBUpdate(s_chat_tb, delta_time);
    TBUpdate(s_search_tb, delta_time);

    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
        },
        .backgroundColor = C_BG_GREY
    }) {
        const int prev_active = s_active_conv;

        SideBar(s_conversations, s_conv_count, &s_active_conv, s_search_tb);

        /* Clear unread when switching conversations */
        if (s_active_conv != prev_active &&
            s_active_conv >= 0 && s_active_conv < s_conv_count &&
            s_conversations[s_active_conv]) {
            ConvSetUnreadCount(s_conversations[s_active_conv], 0);
        }

        /* Render active conversation */
        if (s_active_conv >= 0 && s_active_conv < s_conv_count) {
            MainPanel(s_chat_tb, s_conversations[s_active_conv], s_me);
        }
    }

    const Clay_RenderCommandArray renderCommands = Clay_EndLayout(delta_time);
    UI_Layout(renderCommands);
}

void ChatRender(SDL_Renderer *renderer)
{
    (void)renderer;
    /* Rendering is handled by UI_Layout in ChatUpdate */
}
