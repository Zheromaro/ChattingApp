#include "LoopLogic/UI.h"
#include "AppLogic/SideBar.h"
#include "AppLogic/MainPanel.h"


void ChatUpdate(float delta_time) {
    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
        },
        .backgroundColor = { 231, 235, 240, 255 }
    }) {
        SideBar();
        MainPanel();
    }

    // Capture the processed commands layout and assign back to UI system
    Clay_RenderCommandArray renderCommands = Clay_EndLayout(delta_time);
    UI_Layout(renderCommands);
}
