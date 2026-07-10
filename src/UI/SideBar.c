#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <clay.h>

#include "Core/UI.h"
#include "UI/UI_Event.h"
#include "UI/SideBar.h"
#include "UI/Widgets/Avatar.h"
#include "UI/Widgets/Badge.h"
#include "UI/Widgets/TextBox.h"
#include "UI/Logic/TextBox.h"
#include "UI/UI_Theme.h"
#include "Model/Conversation.h"
#include "Model/User.h"
#include "Model/Message.h"
#include "Model/ContactList.h"

/* ── Internal state ─────────────────────────────────────────────── */

static int          s_hovered_contact = -1;
static ContactList *s_contact_list    = NULL;

/* ── Helpers ───────────────────────────────────────────────────── */

static const char* GetContactName(const Conversation* conv)
{
    if (!conv) return "Unknown";
    const User* u = ConvGetParticipant(conv, 0);
    return (u && UserGetName(u)) ? UserGetName(u) : "Unknown";
}

static const char* GetLastMessage(const Conversation* conv)
{
    if (!conv) return "No messages yet";
    const size_t count = ConvGetMessageCount(conv);
    if (count == 0) return "No messages yet";
    const Message* msg = ConvGetMessage(conv, count - 1);
    return (msg && MessageGetText(msg)) ? MessageGetText(msg) : "";
}

static int GetUnreadCount(const Conversation* conv)
{
    if (!conv) return 0;
    return (int)ConvGetUnreadCount(conv);
}

/* ── Click handler ────────────────────────────────────────────── */

static void HandleContactClick(Clay_ElementId elementId,
                               Clay_PointerData pointerData,
                               void *userData)
{
    (void)elementId;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        int index = (int)(uintptr_t)userData;
        if (s_contact_list) {
            CLSetActiveIndex(s_contact_list, index);
        }
    }
}

/* ── Contact row ───────────────────────────────────────────────── */

static void NameText(const char* name, bool isActive)
{
    CLAY_TEXT(CLAY_STR(name), {
        .fontId   = FONT_ID_BODY_16,
        .fontSize = 16,
        .textColor = isActive ? C_WHITE : C_BLACK
    });
}

static void LastMessageText(const char* text, bool isActive)
{
    CLAY_TEXT(CLAY_STR(text), {
        .fontId   = FONT_ID_BODY_13,
        .fontSize = 13,
        .textColor = isActive ? (Clay_Color){215, 240, 255, 255} : C_PLACEHOLDER
    });
}

static void Contact(int i, Conversation* conv)
{
    if (!conv || !s_contact_list) return;

    const bool isActive = (i == CLGetActiveIndex(s_contact_list));
    const char* name    = GetContactName(conv);
    const char* lastMsg = GetLastMessage(conv);
    const int   unread  = GetUnreadCount(conv);

    CLAY(CLAY_IDI("Contact", i), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .padding = {.left = 12, .right = 12, .top = 10, .bottom = 10},
            .childGap = 12,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
            .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = isActive ? C_BLUE :
                          Clay_Hovered() ? (Clay_Color){220, 220, 230, 255} :
                          C_WHITE
    }) {
        const bool isHovered = Clay_Hovered();
        Clay_OnHover(HandleContactClick, (void *)(uintptr_t)i);

        if (isHovered) s_hovered_contact = i;

        Avatar(i, name, isActive, isHovered);

        CLAY(CLAY_IDI("TextStack", i), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .childGap = 4,
                .sizing = {.width = CLAY_SIZING_GROW(0)}
            }
        }) {
            NameText(name, isActive);
            LastMessageText(lastMsg, isActive);
        }

        if (!isActive && unread > 0) {
            UnreadBadge(unread);
        }
    }
}

static void ContactListView(Conversation **conversations, int count,
                            const char* filter)
{
    CLAY(CLAY_ID("ContactList"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}
        },
        .clip = {.vertical = true, .childOffset = Clay_GetScrollOffset()}
    }) {
        for (int i = 0; i < count; i++) {
            if (!conversations[i]) continue;

            if (filter && filter[0]) {
                const char* name = GetContactName(conversations[i]);
                if (!strstr(name, filter)) continue;
            }

            Contact(i, conversations[i]);
        }
    }
}

/* ── Search bar ───────────────────────────────────────────────── */

static void SearchBar(const TextBox* tb, UI_Event* event)
{
    const bool isFocused = tb ? true : false;  /* TODO: wire to real focus state */

    CLAY(CLAY_ID("SearchBar"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .padding = CLAY_PADDING_ALL(14),
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
            .childAlignment = {.y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = isFocused ? C_WHITE :
                          Clay_Hovered() ? (Clay_Color){235, 235, 240, 255} :
                          C_INPUT_BG,
        .border = isFocused ? (Clay_BorderElementConfig){
            .width = {.bottom = 2},
            .color = C_BLUE
        } : (Clay_BorderElementConfig){0}
    }) {
        const bool isHovered = Clay_Hovered();
        if (isHovered && UI_GetMouseDown()) event->searchBox();

        if (tb) {
            TextInputContent((TextBox*)tb, "Search conversations...",
                             15, C_BLACK, C_PLACEHOLDER, FONT_ID_BODY_15);
        } else {
            CLAY_TEXT(CLAY_STRING("Search conversations..."), {
                .fontId   = FONT_ID_BODY_15,
                .fontSize = 15,
                .textColor = C_PLACEHOLDER
            });
        }
    }
}

void SideBar(ContactList *cl, const TextBox *search_tb, UI_Event* event)
{
    s_contact_list    = cl;
    s_hovered_contact = -1;

    const char* filter = search_tb ? TBGetText((TextBox*)search_tb) : "";

    Conversation **conversations = cl ? CLGetConversationsArray(cl) : NULL;
    const int      count         = cl ? CLGetCount(cl) : 0;

    CLAY(CLAY_ID("SideBar"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_FIXED(320), .height = CLAY_SIZING_GROW(0)}
        },
        .backgroundColor = C_WHITE
    }) {
        SearchBar(search_tb, event);
        if (conversations && count > 0) {
            ContactListView(conversations, count, filter);
        }
    }
}
