#include <stdio.h>
#include "LoopLogic/UI.h"


void ChatUpdate(float delta_time) {
    // Build the UI frame
    Clay_BeginLayout();

    // Container to center everything

    // An outer container that grows to fill available space
    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        },
        .backgroundColor = {250, 250, 255, 255}
    }) {

        // A fixed-width sidebar (300px) that lays its children out vertically
        CLAY(CLAY_ID("SideBar"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW(0) },
                .childGap = 16
            },
            .backgroundColor = {224, 215, 210, 255}
        }) {
            // Text is rendered using the CLAY_TEXT macro
            CLAY_TEXT(CLAY_STRING("My Sidebar"), { .fontSize = 24, .textColor = {0, 0, 0, 255} });


        }
    }

    // End layout and render
    Clay_RenderCommandArray renderCommands = Clay_EndLayout(delta_time);

    UI_Layout(renderCommands);
}
