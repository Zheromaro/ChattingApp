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
TTF_Font *font = NULL;

static float mouseX = 0.0f;
static float mouseY = 0.0f;
static float scrollX = 0.0f;
static float scrollY = 0.0f;
static bool mouseDown = false;

static void HandleClayErrors(Clay_ErrorData errorData) {
    fprintf(stderr, "Clay Error [%d]: %s\n", errorData.errorType, errorData.errorText.chars);
}

static Clay_Dimensions Clay_SDL3_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
    font = clayRenderer.fonts[config->fontId];
    if (font == NULL) return (Clay_Dimensions){0, 0};

    int extent = 0;
    TTF_MeasureString(font, text.chars, text.length, 100000, &extent, NULL);

    int height = TTF_GetFontHeight(font);

    if (config->letterSpacing > 0 && text.length > 1) {
        extent += (text.length - 1) * config->letterSpacing;
    }

    if (config->lineHeight > 0) {
        height = config->lineHeight;
    }

    return (Clay_Dimensions){ (float)extent, (float)height };
}

bool UI_Init(int width, int height, SDL_Renderer *renderer, TTF_TextEngine *textEngine, TTF_Font **fonts) {
    if (renderer == NULL|| textEngine == NULL|| fonts == NULL) {
        fprintf(stderr, "UI_Init: Invalid game state or missing SDL resources");
        return false;
    }

    windowWidth = width;
    windowHeight = height;

    // Setup the Clay SDL3 renderer data
    clayRenderer = (Clay_SDL3RendererData){
        .renderer = renderer,
        .textEngine = textEngine,
        .fonts = fonts,
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

void UI_Free(void) {
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
    SDL_GetRenderOutputSize(clayRenderer.renderer, &windowWidth, &windowHeight);
    Clay_SetLayoutDimensions((Clay_Dimensions){ (float)windowWidth, (float)windowHeight });
    Clay_SetPointerState((Clay_Vector2){ mouseX, mouseY }, mouseDown);
    Clay_UpdateScrollContainers(true, (Clay_Vector2){ scrollX, scrollY }, deltaTime);

    scrollX = 0.0f;
    scrollY = 0.0f;
}

void UI_Render(void) {
    SDL_Clay_RenderClayCommands(&clayRenderer, &commands); // apply CLAY commands
}

// getters
float UI_GetMouseX(void) {
    return mouseX;
}

float UI_GetMouseY(void) {
    return mouseY;
}

bool UI_GetMouseDown(void) {
    return mouseDown;
}
