#include "States.h"
#define CLAY_IMPLEMENTATION
#include <stdlib.h>
#include <stdio.h>
#include <clay.h>
#include <clay_renderer_SDL3.c>
#include "LoopLogic/UI.h"
#include "LoopFunc.h"

#define CLAY_MEMORY_SIZE 1024 * 1024  // 1MB for Clay arena

static Clay_SDL3RendererData clayRenderer = {0};
static void *clayMemory = NULL;
static int windowWidth = 0;
static int windowHeight = 0;
static Game *gameInstance = NULL;
static Clay_RenderCommandArray *commands = NULL;

static float mouseX = 0.0f;
static float mouseY = 0.0f;
static bool mouseDown = false;

static void HandleClayErrors(Clay_ErrorData errorData) {
    fprintf(stderr, "[Clay Error] %s", errorData.errorText.chars);
}

bool UI_Init(int width, int height, Game *g) {
    if (!g || !g->renderer || !g->textEngine || !g->fonts) {
        fprintf(stderr, "UI_Init: Invalid game state or missing SDL resources");
        return false;
    }

    gameInstance = g;
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

    Clay_Initialize(arena, (Clay_Dimensions){ width, height }, (Clay_ErrorHandler){0});

    return true;
}

void UI_Free() {
    if (clayMemory != NULL) {
        free(clayMemory);
        clayMemory = NULL;
    }
    gameInstance = NULL;
}



static void UI_Input(SDL_Event e) {
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_EVENT_QUIT:
                gameInstance->running = false;
                break;
            case SDL_EVENT_MOUSE_MOTION:
                mouseX = e.motion.x;
                mouseY = e.motion.y;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (e.button.button == SDL_BUTTON_LEFT) mouseDown = true;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (e.button.button == SDL_BUTTON_LEFT) mouseDown = false;
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                Clay_UpdateScrollContainers(true, (Clay_Vector2){ e.wheel.x, e.wheel.y }, 0.016f);
                break;
        }
    }
}

static void UI_Update(float dt) {
    SDL_GetWindowSize(gameInstance->window, &windowWidth, &windowHeight);
    Clay_SetLayoutDimensions((Clay_Dimensions){ (float)windowWidth, (float)windowHeight });
    Clay_SetPointerState((Clay_Vector2){ mouseX, mouseY }, mouseDown);
}

static void UI_Render(SDL_Renderer *renderer) {
    SDL_Clay_RenderClayCommands(&clayRenderer, commands); // apply CLAY commands
}

State ui = {
    .Update = UI_Update,
    .Input = UI_Input,
    .Render = UI_Render,
};
