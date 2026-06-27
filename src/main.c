#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "SETTINGS.h"
#include "LoopFunc.h"
#include "LoopLogic/UI.h"


#define SDL_FLAGS SDL_INIT_VIDEO
#define WINDOW_TITLE "Zahrawi"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


bool init_app(Game *g) {
    if (!SDL_Init(SDL_FLAGS)) {
        fprintf(stderr, "Error initializing SDL3: %s\n", SDL_GetError());
        return false;
    }

    g->window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
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

void loop(Game *g) {
    Enter();
    while (g->running) {
        Input(g);
        Update();
        Render(g->renderer);
    }
    Exit();
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
