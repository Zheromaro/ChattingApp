#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Core/UI.h"
#include "Controller/Chat.h"
#include "UI/SideBar.h"
#include "UI/MainPanel.h"
#include "UI/Logic/TextBox.h"
#include "UI/Widgets/TextBox.h"
#include "UI/UI_Theme.h"
#include "Model/Message.h"
#include "Model/User.h"
#include "Model/Conversation.h"
#include "Model/ContactList.h"

static TextBox* s_chat_tb = NULL;
static TextBox* s_search_tb = NULL;
static User* s_me = NULL;
static ContactList* s_contact_list = NULL;
bool chatTextbox = true;
int previous_active_idx = 0;
int current_active_idx = 0;

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

    for (int i = 0; i < 8; i++) {
        Conversation* conv = ConvCreate();
        if (!conv) continue;

        User* contact = UserCreate(names[i]);
        if (contact) {
            ConvAddParticipant(conv, contact);
        }

        Message* msg = MessageCreate(UserGetID(contact), ConvGetID(conv), lastMessages[i]);
        if (msg) {
            ConvAddMessage(conv, msg);
        }

        ConvSetUnreadCount(conv, (size_t)unread[i]);

        // Add to our contact list model
        CLAddConversation(s_contact_list, conv);
    }
}

void ChatEnter(void)
{
    s_chat_tb = TBCreate();
    s_search_tb = TBCreate();
    s_me = UserCreate("zahrawi");
    s_contact_list = CLCreate();

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

    CLDestroy(s_contact_list); // Free ContactList and its contents
    s_contact_list = NULL;
}

void ChatInput(SDL_Event* event)
{
    if (!event) return;

    if (chatTextbox) {
        TBHandleEvent(s_chat_tb, event);
    } else {
        TBHandleEvent(s_search_tb, event);
    }

    /* Global shortcuts */
    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            case SDLK_RETURN:
            case SDLK_KP_ENTER: {
                const char* text = TBTakeText(s_chat_tb);
                if (!text || text[0] == '\0') break;

                // Safely grab the active conversation
                Conversation* active_conv = CLGetActiveConversation(s_contact_list);
                if (!active_conv) break;

                Message* msg = MessageCreate(UserGetID(s_me), ConvGetID(active_conv), text);
                if (msg) {
                    ConvAddMessage(active_conv, msg);
                }
                break;
            }
        }
    }
}

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
        // Bridge state for the SideBar to manipulate
        previous_active_idx = current_active_idx;
        current_active_idx = CLGetActiveIndex(s_contact_list);

        SideBar(s_contact_list, s_search_tb);

        /* Handle conversation switching and clear unread counts */
        if (current_active_idx != previous_active_idx) {
            CLSetActiveIndex(s_contact_list, current_active_idx);

            Conversation* new_active_conv = CLGetActiveConversation(s_contact_list);
            if (new_active_conv) {
                ConvSetUnreadCount(new_active_conv, 0);
            }
        }

        /* Render active conversation */
        Conversation* active_conv = CLGetActiveConversation(s_contact_list);
        if (active_conv) {
            MainPanel(s_chat_tb, active_conv, s_me);
        }
    }

    const Clay_RenderCommandArray renderCommands = Clay_EndLayout(delta_time);
    UI_Layout(renderCommands);
}
