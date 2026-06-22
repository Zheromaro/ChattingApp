#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "LoopLogic/UI.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


#define SDL_FLAGS SDL_INIT_VIDEO
#define WINDOW_TITLE "Zahrawi"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


bool init_app(Game *g) {
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

    // UI
    if (!UI_Init(WINDOW_WIDTH, WINDOW_HEIGHT, g)) {
        fprintf(stderr, "Error initializing UI\n");
        return false;
    }

    g->running = true;
    return true;
}

void free_app(Game *g) {
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
    UI_Free();
    TTF_Quit();
    SDL_Quit();
}

// loop functions
void input(Game *g) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_EVENT_QUIT:
                g->running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (e.key.key == SDLK_ESCAPE)
                    g->running = false;
                break;
        }
    }
}

Uint64 last_tick = 0;
Uint64 now = 0;
Uint64 frame_time = 0;
Uint64 target_ns = 1000000000ULL / 60;   // 60 FPS cap (optional)
float dt = 0;
void update() {
    now = SDL_GetTicksNS();
    dt   = (now - last_tick) / 1000000000.0f;  /* seconds */
    last_tick  = now;

    // safety clamp: if we hit a breakpoint dt doesn't explode
    if (dt > 0.25f) dt = 0.25f;

    UI_Update();

    // frame cap
    frame_time = SDL_GetTicksNS() - now;
    if (frame_time < target_ns) {
        SDL_DelayNS(target_ns - frame_time);
    }
}

void render(Game *g) {
    // clear background
    SDL_SetRenderDrawColor(g->renderer, 30, 30, 30, 255);
    SDL_RenderClear(g->renderer);

    /* draw here: SDL_RenderRect(), TTF_RenderText_Blended(), UI */

    // push to screen
    SDL_RenderPresent(g->renderer);
}

void loop(Game *g) {
    while (g->running) {
        input(g);
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
        // --- End UI ---
        update();
        UI_Render(&commands);

    }
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    Game game = {0};

    if (!init_app(&game)) {
        free_app(&game);
        return EXIT_FAILURE;
    }
    loop(&game);
    free_app(&game);
    return EXIT_SUCCESS;
}
