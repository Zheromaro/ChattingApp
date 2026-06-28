#include <clay.h>
#include <stdbool.h>
#include "AppLogic/UI/SideBar.h"
#include "AppLogic/UI/CONST_UI.h" // FIXED: Included constants for fonts

void Avatar(int i, bool isActiveChat) {
    CLAY(CLAY_IDI("Avatar", i), {
        .layout = { .sizing = { .width = CLAY_SIZING_FIXED(44), .height = CLAY_SIZING_FIXED(44) } },
        .backgroundColor = isActiveChat ? (Clay_Color){ 255, 255, 255, 255 } : (Clay_Color){ 200, 205, 210, 255 },
        .cornerRadius = { .topLeft = 22, .topRight = 22, .bottomLeft = 22, .bottomRight = 22 }
    }) {}
}

void Name(bool isActiveChat) {
    CLAY_TEXT(
        CLAY_STRING("Alex"),
        // FIXED: Added FONT_ID_BODY_16
        { .fontId = FONT_ID_BODY_16, .fontSize = 16, .textColor = isActiveChat ? (Clay_Color){255,255,255,255} : (Clay_Color){0,0,0,255} }
    );
}

void LastMessage(bool isActiveChat) {
    CLAY_TEXT(
        CLAY_STRING("Typing dynamic layout rules..."),
        // FIXED: Added FONT_ID_BODY_13
        { .fontId = FONT_ID_BODY_13, .fontSize = 13, .textColor = isActiveChat ? (Clay_Color){215,240,255,255} : (Clay_Color){140,140,145,255} }
    );
}

void Contact(int i, bool isActiveChat) {
    CLAY(CLAY_IDI("Contact", i), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .padding = CLAY_PADDING_ALL(12),
            .childGap = 12,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = isActiveChat ? (Clay_Color){ 44, 147, 227, 255 } : (Clay_Color){ 255, 255, 255, 255 }
    }) {
        Avatar(i, isActiveChat);

        CLAY(CLAY_IDI("TextStack", i), {
            .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 4 }
        }) {
            Name(isActiveChat);
            LastMessage(isActiveChat);
        }
    }
}

void ContactList(void) {
    CLAY(CLAY_ID("ContactList"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
        }
    }) {
        for (int i = 0; i < 6; i++) {
            bool isActiveChat = (i == 0);
            Contact(i, isActiveChat);
        }
    }
}

void Search(void) {
    CLAY(CLAY_ID("SidebarHeader"), {
        .layout = {
            .padding = CLAY_PADDING_ALL(14),
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) }
        },
        .backgroundColor = { 244, 244, 245, 255 }
    }) {
        // FIXED: Added FONT_ID_BODY_15
        CLAY_TEXT(CLAY_STRING("Search conversations..."), { .fontId = FONT_ID_BODY_15, .fontSize = 15, .textColor = { 142, 142, 147, 255 } });
    }
}

void SideBar(void) {
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
