#include <clay.h>
#include "AppLogic/SideBar.h"

void SideBar() {
    CLAY(CLAY_ID("SideBar"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIXED(320), .height = CLAY_SIZING_GROW(0) }
        },
        .backgroundColor = { 255, 255, 255, 255 } // Pure white sidebar panel
    }) {

        // Sidebar Search/Header Mock
        CLAY(CLAY_ID("SidebarHeader"), {
            .layout = {
                .padding = CLAY_PADDING_ALL(14),
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) }
            },
            .backgroundColor = { 244, 244, 245, 255 }
        }) {
            CLAY_TEXT(CLAY_STRING("Search conversations..."), { .fontSize = 15, .textColor = { 142, 142, 147, 255 } });
        }

        // Conversations List Container
        CLAY(CLAY_ID("ConversationsContainer"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
            }
        }) {
            // Loop to mock multiple conversation rows
            for (int i = 0; i < 6; i++) {
                bool isActiveChat = (i == 0); // Mock the first chat item as currently open/selected

                CLAY(CLAY_IDI("ConversationItem", i), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .padding = CLAY_PADDING_ALL(12),
                        .childGap = 12,
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = isActiveChat ? (Clay_Color){ 44, 147, 227, 255 } : (Clay_Color){ 255, 255, 255, 255 }
                }) {
                    // User Avatar circular placeholder
                    CLAY(CLAY_IDI("Avatar", i), {
                        .layout = { .sizing = { .width = CLAY_SIZING_FIXED(44), .height = CLAY_SIZING_FIXED(44) } },
                        .backgroundColor = isActiveChat ? (Clay_Color){ 255, 255, 255, 255 } : (Clay_Color){ 200, 205, 210, 255 },
                        .cornerRadius = { .topLeft = 22, .topRight = 22, .bottomLeft = 22, .bottomRight = 22 }
                    }) {}

                    // Name & Last message stack
                    CLAY(CLAY_IDI("TextStack", i), {
                        .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 4 }
                    }) {
                        CLAY_TEXT(
                            isActiveChat ? CLAY_STRING("Alex") : CLAY_STRING("Community Channel"),
                            { .fontSize = 16, .textColor = isActiveChat ? (Clay_Color){255,255,255,255} : (Clay_Color){0,0,0,255} }
                        );
                        CLAY_TEXT(
                            CLAY_STRING("Typing dynamic layout rules..."),
                            { .fontSize = 13, .textColor = isActiveChat ? (Clay_Color){215,240,255,255} : (Clay_Color){140,140,145,255} }
                        );
                    }
                }
            }
        }
    }
}
