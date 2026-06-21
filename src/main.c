#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_renderer_SDL3.c"

#define SDL_FLAGS SDL_INIT_VIDEO
#define WINDOW_TITLE "Zahrawi"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CLAY_MEMORY_SIZE 1024 * 1024  // 1MB for Clay arena

typedef struct Game {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_TextEngine *textEngine;
    TTF_Font **fonts;
    Clay_SDL3RendererData clayRenderer;
    bool running;
} Game;

bool init_sdl(Game *g) {
    if (!SDL_Init(SDL_FLAGS)) {
        fprintf(stderr, "Error initializing SDL3: %s\n", SDL_GetError());
        return false;
    }

    g->window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (g->window == NULL) {
        fprintf(stderr, "Error Creating Window: %s\n", SDL_GetError());
        return false;
    }

    g->renderer = SDL_CreateRenderer(g->window, NULL);
    if (g->renderer == NULL) {
        fprintf(stderr, "Error Creating Renderer: %s\n", SDL_GetError());
        return false;
    }
    SDL_SetRenderVSync(g->renderer, 1);

    // SDL_ttf
    if (!TTF_Init()) {
        fprintf(stderr, "Error initializing SDL_ttf: %s\n", SDL_GetError());
        return false;
    }

    g->textEngine = TTF_CreateRendererTextEngine(g->renderer);
    if (g->textEngine == NULL) {
        fprintf(stderr, "Text engine failed: %s\n", SDL_GetError());
        return false;
    }

    // font
    TTF_Font *font = TTF_OpenFont("assets/bit_font.ttf", 24);
    if (font == NULL) {
        fprintf(stderr, "Font load failed: %s\n", SDL_GetError());
        return false;
    }

    g->fonts = malloc(sizeof(TTF_Font *) * 1);
    g->fonts[0] = font;

    // Clay renderer data
    g->clayRenderer = (Clay_SDL3RendererData){
        .renderer = g->renderer,
        .textEngine = g->textEngine,
        .fonts = g->fonts,
    };

    // Initialize Clay
    size_t clayMemSize = Clay_MinMemorySize();
    void *clayMemory = malloc(SDL_max(CLAY_MEMORY_SIZE, clayMemSize));
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(SDL_max(CLAY_MEMORY_SIZE, clayMemSize), clayMemory);

    Clay_Initialize(arena, (Clay_Dimensions){ WINDOW_WIDTH, WINDOW_HEIGHT }, (Clay_ErrorHandler){0});

    g->running = true;
    return true;
}

void loop(Game *g) {
    float mouseX = 0, mouseY = 0;
    bool mouseDown = false;

    while (g->running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    g->running = false;
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

        // --- Update Clay input state ---
        int w, h;
        SDL_GetWindowSize(g->window, &w, &h);
        Clay_SetLayoutDimensions((Clay_Dimensions){ (float)w, (float)h });
        Clay_SetPointerState((Clay_Vector2){ mouseX, mouseY }, mouseDown);

        // --- Build UI ---
        Clay_BeginLayout();

        CLAY(CLAY_ID("MainContainer"), {
            .layout = {
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = { 40, 40, 40, 255 }
        }) {
            CLAY(CLAY_ID("HelloText"), {
                .layout = { .padding = { 20, 20, 10, 10 } },
                .backgroundColor = { 80, 120, 200, 255 },
                .cornerRadius = { 8, 8, 8, 8 }
            }) {
                CLAY_TEXT(CLAY_STRING("Hello from Clay + SDL3!"), CLAY_TEXT_CONFIG({
                    .fontId = 0,
                    .fontSize = 24,
                    .textColor = { 255, 255, 255, 255 }
                }));
            }
        }

        Clay_RenderCommandArray commands = Clay_EndLayout(1);

        // --- Render ---
        SDL_SetRenderDrawColor(g->renderer, 30, 30, 30, 255);
        SDL_RenderClear(g->renderer);

        SDL_Clay_RenderClayCommands(&g->clayRenderer, &commands);

        SDL_RenderPresent(g->renderer);
    }
}

void free_sdl(Game *g) {
    if (g->fonts) {
        for (int i = 0; i < 1; i++) {
            TTF_CloseFont(g->fonts[i]);
            g->fonts[i] = NULL;
        }
        free(g->fonts);
        g->fonts = NULL;
    }
    if (g->textEngine != NULL) {
        TTF_DestroyRendererTextEngine(g->textEngine);
        g->textEngine = NULL;
    }
    if (g->renderer != NULL) {
        SDL_DestroyRenderer(g->renderer);
        g->renderer = NULL;
    }
    if (g->window != NULL) {
        SDL_DestroyWindow(g->window);
        g->window = NULL;
    }
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    Game game = {0};

    if (!init_sdl(&game)) {
        free_sdl(&game);
        return EXIT_FAILURE;
    }
    loop(&game);
    free_sdl(&game);
    return EXIT_SUCCESS;
}
