#define CLAY_IMPLEMENTATION
#include <stdlib.h>
#include <stdio.h>
#include "LoopLogic/UI.h"
#include <clay_renderer_SDL3.c>

#define CLAY_MEMORY_SIZE 1024 * 1024  // 1MB for Clay arena

static Clay_SDL3RendererData clayRenderer = {0};
static void *clayMemory = NULL;
static int windowWidth = 0;
static int windowHeight = 0;
static Clay_RenderCommandArray commands = {0};

static float mouseX = 0.0f;
static float mouseY = 0.0f;
static float scrollX = 0.0f;
static float scrollY = 0.0f;
static bool mouseDown = false;

static void HandleClayErrors(Clay_ErrorData errorData) {
    fprintf(stderr, "Clay Error [%d]: %s\n", errorData.errorType, errorData.errorText.chars);
}

static Clay_Dimensions Clay_SDL3_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
    Clay_SDL3RendererData *rendererData = (Clay_SDL3RendererData*)userData;
    TTF_Font *font = rendererData->fonts[config->fontId];
    if (!font) return (Clay_Dimensions){0, 0};

    // Set the correct font size for this measurement
    TTF_SetFontSize(font, config->fontSize);

    // Create a temporary text object to measure it.
    // Note: Clay_StringSlice is NOT null-terminated, but TTF_CreateText takes a length.
    TTF_Text *textObj = TTF_CreateText(rendererData->textEngine, font, text.chars, text.length);
    if (!textObj) return (Clay_Dimensions){0, 0};

    int w = 0, h = 0;
    TTF_GetTextSize(textObj, &w, &h);
    TTF_DestroyText(textObj);

    // Apply letter spacing if configured
    if (config->letterSpacing > 0 && text.length > 1) {
        w += (text.length - 1) * config->letterSpacing;
    }

    // Apply explicit line height if configured, otherwise use measured height
    if (config->lineHeight > 0) {
        h = config->lineHeight;
    }

    return (Clay_Dimensions){ (float)w, (float)h };
}

bool UI_Init(int width, int height, Game *g) {
    if (!g || !g->renderer || !g->textEngine || !g->fonts) {
        fprintf(stderr, "UI_Init: Invalid game state or missing SDL resources");
        return false;
    }

    windowWidth = width;
    windowHeight = height;

    // Setup the Clay SDL3 renderer data
    clayRenderer = (Clay_SDL3RendererData){
        .renderer = g->renderer,
        .textEngine = g->textEngine,
        .fonts = g->fonts,
    };

    // Initialize Clay
    size_t clayMemSize = Clay_MinMemorySize();
    clayMemory = malloc(SDL_max(CLAY_MEMORY_SIZE, clayMemSize));
    if (clayMemory == NULL) {
        fprintf(stderr, "UI_Init: Failed to allocate Clay memory");
        return false;
    }

    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(SDL_max(CLAY_MEMORY_SIZE, clayMemSize), clayMemory);

    Clay_Initialize(arena, (Clay_Dimensions){ width, height }, (Clay_ErrorHandler){.errorHandlerFunction = HandleClayErrors});

    Clay_SetMeasureTextFunction(Clay_SDL3_MeasureText, &clayRenderer);

    return true;
}

void UI_Free() {
    if (clayMemory != NULL) {
        free(clayMemory);
        clayMemory = NULL;
    }
}

void UI_Layout(Clay_RenderCommandArray command) {
    commands = command;
}

void UI_Input(SDL_Event* e) {
    switch (e->type) {
        case SDL_EVENT_WINDOW_RESIZED:
            windowWidth = e->window.data1;
            windowHeight = e->window.data2;
            break;
        case SDL_EVENT_MOUSE_MOTION:
            mouseX = e->motion.x;
            mouseY = e->motion.y;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (e->button.button == SDL_BUTTON_LEFT) mouseDown = true;
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (e->button.button == SDL_BUTTON_LEFT) mouseDown = false;
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            // Accumulate scroll deltas
            scrollX += e->wheel.x;
            scrollY += e->wheel.y;
            break;
    }
}

void UI_Update(float deltaTime) {
    Clay_SetLayoutDimensions((Clay_Dimensions){ (float)windowWidth, (float)windowHeight });
    Clay_SetPointerState((Clay_Vector2){ mouseX, mouseY }, mouseDown);
    Clay_UpdateScrollContainers(true, (Clay_Vector2){ scrollX, scrollY }, deltaTime);

    scrollX = 0.0f;
    scrollY = 0.0f;
}

void UI_Render() {
    SDL_Clay_RenderClayCommands(&clayRenderer, &commands); // apply CLAY commands
}

// getters
float UI_GetMouseX() {
    return mouseX;
}

float UI_GetMouseY() {
    return mouseY;
}

bool UI_GetMouseDown() {
    return mouseDown;
}
