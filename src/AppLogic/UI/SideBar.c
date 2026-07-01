#include <clay.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "AppLogic/UI/SideBar.h"
#include "AppLogic/UI/CONST_UI.h"


void HandleContactClick(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleSearchClick(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);

// --- State ---
static int activeContactIndex = 0;
static int hoveredContactIndex = -1;

// --- Data ---
typedef struct {
    const char *name;
    const char *lastMessage;
    const char *avatarColor;
    int unreadCount;
} ContactData;

static ContactData contacts[] = {
    {"Alex",      "Typing dynamic layout rules...",  "FF6B6B", 0},
    {"Sarah",     "See you tomorrow!",               "4ECDC4", 2},
    {"Mike",      "Can you review the PR?",          "45B7D1", 0},
    {"Emma",      "Thanks for the help!",            "96CEB4", 5},
    {"David",     "Meeting at 3pm",                  "FFEAA7", 0},
    {"Lisa",      "Sent the files",                  "DDA0DD", 1},
    {"Tom",       "Sounds good to me",               "98D8C8", 0},
    {"Jenny",     "Let me check...",                 "F7DC6F", 3},
};
#define CONTACT_COUNT (sizeof(contacts) / sizeof(contacts[0]))

// --- Components ---

void Avatar(int i, bool isActiveChat, bool isHovered) {
    ContactData *c = &contacts[i];
    Clay_Color bg = isActiveChat ? C_WHITE : HexColor(c->avatarColor);

    // Scale up slightly on hover
    float size = (isHovered && !isActiveChat) ? 48.0f : 44.0f;

    CLAY(CLAY_IDI("Avatar", i), {
        .layout = {
            .sizing = { .width = CLAY_SIZING_FIXED(size), .height = CLAY_SIZING_FIXED(size) },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = bg,
        .cornerRadius = CLAY_CORNER_RADIUS(size / 2)
    }) {
        // Initials as text avatar
        char initials[3] = {c->name[0], '\0', '\0'};
        CLAY_TEXT(CLAY_STR(initials), {
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = isActiveChat ? HexColor(c->avatarColor) : C_WHITE
        });
    }
}

void UnreadBadge(int count) {
    if (count <= 0) return;

    char buf[4];
    snprintf(buf, sizeof(buf), "%d", count > 99 ? 99 : count);

    CLAY(CLAY_ID_LOCAL("UnreadBadge"), {
        .layout = {
            .sizing = { .width = CLAY_SIZING_FIXED(20), .height = CLAY_SIZING_FIXED(20) },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = (Clay_Color){ 255, 59, 48, 255 }, // iOS red
        .cornerRadius = CLAY_CORNER_RADIUS(10)
    }) {
        CLAY_TEXT(CLAY_STR(buf), {
            .fontId = FONT_ID_BODY_13,
            .fontSize = 11,
            .textColor = C_WHITE
        });
    }
}

void Name(int i, bool isActiveChat) {
    CLAY_TEXT(
        CLAY_STR(contacts[i].name),
        { .fontId = FONT_ID_BODY_16, .fontSize = 16, .textColor = isActiveChat ? C_WHITE : C_BLACK }
    );
}

void LastMessage(int i, bool isActiveChat) {
    CLAY_TEXT(
        CLAY_STR(contacts[i].lastMessage),
        {
            .fontId = FONT_ID_BODY_13,
            .fontSize = 13,
            .textColor = isActiveChat ? (Clay_Color){215,240,255,255} : C_PLACEHOLDER
        }
    );
}

void Contact(int i) {
    bool isActive = (i == activeContactIndex);

    CLAY(CLAY_IDI("Contact", i), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .padding = { .left = 12, .right = 12, .top = 10, .bottom = 10 },
            .childGap = 12,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = isActive ? C_BLUE :
                          Clay_Hovered() ? (Clay_Color){ 220, 220, 230, 255 } :
                          (Clay_Color){ 255, 255, 255, 255 }
    }) {
        // Register hover handler (handles both hover visual state and click detection)
        Clay_OnHover(HandleContactClick, (void *)(uintptr_t)i);

        // Track hover state during layout
        if (Clay_Hovered()) {
            hoveredContactIndex = i;
        }

        Avatar(i, isActive, (i == hoveredContactIndex));

        CLAY(CLAY_IDI("TextStack", i), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .childGap = 4,
                .sizing = { .width = CLAY_SIZING_GROW(0) }
            }
        }) {
            Name(i, isActive);
            LastMessage(i, isActive);
        }

        // Unread badge on the right
        if (!isActive && contacts[i].unreadCount > 0) {
            UnreadBadge(contacts[i].unreadCount);
        }
    }
}

// --- Click handler ---
void HandleContactClick(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData) {
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        int index = (int)(uintptr_t)userData;
        activeContactIndex = index;
        // TODO: Notify your Chat system to switch conversation
        // ChatSwitchToContact(index);
    }
}

void ContactList(void) {
    CLAY(CLAY_ID("ContactList"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
        },
        .clip = {
            .vertical = true,
            .childOffset = Clay_GetScrollOffset()
        }
    }) {
        for (int i = 0; i < CONTACT_COUNT; i++) {
            Contact(i);
        }
    }
}

// --- Search with focus state ---
static bool searchFocused = false;
static char searchText[64] = "";

void Search(void) {
    CLAY(CLAY_ID("SearchBar"), {
        .layout = {
            .padding = CLAY_PADDING_ALL(14),
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) }
        },
        .backgroundColor = searchFocused ? C_WHITE :
                          Clay_Hovered() ? (Clay_Color){ 235, 235, 240, 255 } :
                          C_INPUT_BG,
        .border = searchFocused ? (Clay_BorderElementConfig){
            .width = { .bottom = 2 },
            .color = C_BLUE
        } : (Clay_BorderElementConfig){0}
    }) {
        // Register hover/click handler
        Clay_OnHover(HandleSearchClick, NULL);

        if (searchFocused) {
            CLAY_TEXT(CLAY_STR(searchText[0] ? searchText : "|"), {
                .fontId = FONT_ID_BODY_15,
                .fontSize = 15,
                .textColor = C_BLACK
            });
        } else {
            CLAY_TEXT(CLAY_STRING("Search conversations..."), {
                .fontId = FONT_ID_BODY_15,
                .fontSize = 15,
                .textColor = C_PLACEHOLDER
            });
        }
    }
}

void HandleSearchClick(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData) {
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        searchFocused = true;
        // TODO: SDL_StartTextInput() to show keyboard on mobile
    }
}

// Call this from UI_Input when SDL_EVENT_TEXT_INPUT fires
void SideBar_HandleTextInput(const char *text) {
    if (!searchFocused) return;
    size_t len = strlen(searchText);
    size_t add = strlen(text);
    if (len + add < sizeof(searchText) - 1) {
        strcat(searchText, text);
    }
}

void SideBar_HandleBackspace(void) {
    if (!searchFocused) return;
    size_t len = strlen(searchText);
    if (len > 0) searchText[len - 1] = '\0';
}

void SideBar_BlurSearch(void) {
    searchFocused = false;
}

void SideBar(void) {
    // Reset hover tracking each frame
    hoveredContactIndex = -1;

    CLAY(CLAY_ID("SideBar"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIXED(320), .height = CLAY_SIZING_GROW(0) }
        },
        .backgroundColor = { 255, 255, 255, 255 }
    }) {
        Search();
        ContactList();
    }
}
