#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <clay.h>

#include "Core/Text.h"
#include "UI/SideBar.h"
#include "UI/CONST_UI.h"
#include "UI/TextBox.h"
#include "Model/Message.h"
#include "Model/Conversation.h"
#include "Model/User.h"

/* ── Internal state ─────────────────────────────────────────────── */

static int  s_hovered_contact = -1;
static int *s_active_index    = NULL;

/* ── Helpers ───────────────────────────────────────────────────── */

static Clay_Color NameToColor(const char* name)
{
    if (!name || !name[0]) {
        return (Clay_Color){ 150, 150, 150, 255 };
    }

    unsigned hash = 5381;
    while (*name) {
        hash = ((hash << 5) + hash) + (unsigned char)*name++;
    }

    static const Clay_Color palette[] = {
        {255, 107, 107, 255}, {78,  205, 196, 255},
        {69,  183, 209, 255}, {150, 206, 180, 255},
        {255, 234, 167, 255}, {221, 160, 221, 255},
        {152, 216, 200, 255}, {247, 220, 111, 255}
    };
    return palette[hash % 8];
}

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

/* ── Components ─────────────────────────────────────────────────── */

static void Avatar(int i, const char* name, bool isActive, bool isHovered)
{
    const Clay_Color bg = isActive ? C_WHITE : NameToColor(name);
    const float size    = (isHovered && !isActive) ? 48.0f : 44.0f;
    const char initial  = (name && name[0]) ? name[0] : '?';

    CLAY(CLAY_IDI("Avatar", i), {
        .layout = {
            .sizing = { .width = CLAY_SIZING_FIXED(size), .height = CLAY_SIZING_FIXED(size) },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = bg,
        .cornerRadius = CLAY_CORNER_RADIUS(size / 2.0f)
    }) {
        const char str[2] = { initial, '\0' };
        CLAY_TEXT(CLAY_STR(str), {
            .fontId   = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = isActive ? NameToColor(name) : C_WHITE
        });
    }
}

static void UnreadBadge(int count)
{
    if (count <= 0) return;

    char buf[8];
    snprintf(buf, sizeof(buf), "%d", count > 99 ? 99 : count);

    CLAY(CLAY_ID_LOCAL("UnreadBadge"), {
        .layout = {
            .sizing = { .width = CLAY_SIZING_FIXED(20), .height = CLAY_SIZING_FIXED(20) },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = (Clay_Color){ 255, 59, 48, 255 },
        .cornerRadius = CLAY_CORNER_RADIUS(10)
    }) {
        CLAY_TEXT(CLAY_STR(buf), {
            .fontId   = FONT_ID_BODY_13,
            .fontSize = 11,
            .textColor = C_WHITE
        });
    }
}

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

/* ── Click handlers ─────────────────────────────────────────────── */

static void HandleContactClick(Clay_ElementId elementId,
                                Clay_PointerData pointerData,
                                void *userData)
{
    (void)elementId;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        const int index = (int)(uintptr_t)userData;
        if (s_active_index) {
            *s_active_index = index;
        }
    }
}

/* ── Contact item ───────────────────────────────────────────────── */

static void Contact(int i, Conversation* conv)
{
    if (!conv || !s_active_index) return;

    const bool isActive  = (i == *s_active_index);
    const bool isHovered = Clay_Hovered();
    const char* name     = GetContactName(conv);
    const char* lastMsg  = GetLastMessage(conv);
    const int   unread   = GetUnreadCount(conv);

    CLAY(CLAY_IDI("Contact", i), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .padding = { .left = 12, .right = 12, .top = 10, .bottom = 10 },
            .childGap = 12,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = isActive  ? C_BLUE :
                           isHovered ? (Clay_Color){ 220, 220, 230, 255 } :
                                       C_WHITE
    }) {
        Clay_OnHover(HandleContactClick, (void *)(uintptr_t)i);

        if (isHovered) {
            s_hovered_contact = i;
        }

        Avatar(i, name, isActive, isHovered);

        CLAY(CLAY_IDI("TextStack", i), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .childGap = 4,
                .sizing = { .width = CLAY_SIZING_GROW(0) }
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

static void ContactList(Conversation **conversations, int count,
                        const char* filter)
{
    CLAY(CLAY_ID("ContactList"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
        },
        .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
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

/* ── Search bar (TextBox-powered) ─────────────────────────────── */

static void SearchBar(const TextBox* tb)
{
    const bool isHovered  = Clay_Hovered();
    const bool isFocused  = tb ? true : false;
    const char* text      = tb ? TBGetText((TextBox*)tb) : "";
    const bool  hasText   = text && text[0];

    CLAY(CLAY_ID("SearchBar"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .padding = CLAY_PADDING_ALL(14),
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = isFocused ? C_WHITE :
                           isHovered  ? (Clay_Color){ 235, 235, 240, 255 } :
                                        C_INPUT_BG,
        .border = isFocused ? (Clay_BorderElementConfig){
            .width = { .bottom = 2 },
            .color = C_BLUE
        } : (Clay_BorderElementConfig){0}
    }) {
        if (isFocused || hasText) {
            const size_t cursor   = tb ? TBGetCursorPos((TextBox*)tb) : 0;
            const size_t len      = text ? strlen(text) : 0;
            const size_t cursor_b = tb ? TBGetByteOffset((TextBox*)tb, cursor) : 0;

            /* Before cursor */
            if (cursor_b > 0) {
                Clay_String before = { .chars = text, .length = (int32_t)cursor_b };
                CLAY_TEXT(before, {
                    .fontId = FONT_ID_BODY_15, .fontSize = 15, .textColor = C_BLACK
                });
            }

            /* Cursor */
            if (isFocused) {
                const Clay_Color cursorColor = TBIsCursorVisible((TextBox*)tb)
                    ? (Clay_Color){ 0, 0, 0, 255 }
                    : (Clay_Color){ 0, 0, 0, 0 };

                CLAY(CLAY_ID("SearchCursor"), {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_FIXED(2), .height = CLAY_SIZING_FIXED(18) },
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = cursorColor
                }) {}
            }

            /* After cursor */
            if (cursor_b < len) {
                Clay_String after = {
                    .chars = text + cursor_b,
                    .length = (int32_t)(len - cursor_b)
                };
                CLAY_TEXT(after, {
                    .fontId = FONT_ID_BODY_15, .fontSize = 15, .textColor = C_BLACK
                });
            }
        } else {
            CLAY_TEXT(CLAY_STRING("Search conversations..."), {
                .fontId   = FONT_ID_BODY_15,
                .fontSize = 15,
                .textColor = C_PLACEHOLDER
            });
        }
    }
}

/* ── Public API ─────────────────────────────────────────────────── */

void SideBar(Conversation **conversations, int count,
             int *active_index, const TextBox *search_tb)
{
    s_active_index = active_index;
    s_hovered_contact = -1;

    const char* filter = search_tb ? TBGetText((TextBox*)search_tb) : "";

    CLAY(CLAY_ID("SideBar"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIXED(320), .height = CLAY_SIZING_GROW(0) }
        },
        .backgroundColor = C_WHITE
    }) {
        SearchBar(search_tb);
        if (conversations && count > 0) {
            ContactList(conversations, count, filter);
        }
    }
}
