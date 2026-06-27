#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "LoopFunc.h"
#include "LoopLogic/UI.h"


#define SDL_FLAGS SDL_INIT_VIDEO
#define WINDOW_TITLE "Zahrawi"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct App {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_TextEngine *textEngine;
    TTF_Font **fonts;
    bool running;
} App;

bool init_app(App *a) {
    if (!SDL_Init(SDL_FLAGS)) {
        fprintf(stderr, "Error initializing SDL3: %s\n", SDL_GetError());
        return false;
    }

    a->window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (a->window == NULL) {
        fprintf(stderr, "Error Creating Window: %s\n", SDL_GetError());
        return false;
    }

    a->renderer = SDL_CreateRenderer(a->window, NULL);
    if (a->renderer == NULL) {
        fprintf(stderr, "Error Creating Renderer: %s\n", SDL_GetError());
        return false;
    }
    SDL_SetRenderVSync(a->renderer, 1);

    // window icon
    const char *driver = SDL_GetCurrentVideoDriver();
    if (!(driver && SDL_strcmp(driver, "wayland") == 0)) {
        SDL_Surface *icon = IMG_Load("assets/appLogo.png");
        if (icon == NULL)
            fprintf(stderr, "Error loading window icon: %s\n", SDL_GetError());
        if (!SDL_SetWindowIcon(a->window, icon))
            fprintf(stderr, "Error setting window icon: %s\n", SDL_GetError());

        SDL_DestroySurface(icon);
    }

    // SDL_ttf
    if (!TTF_Init()) {
        fprintf(stderr, "Error initializing SDL_ttf: %s\n", SDL_GetError());
        return false;
    }

    a->textEngine = TTF_CreateRendererTextEngine(a->renderer);
    if (a->textEngine == NULL) {
        fprintf(stderr, "Text engine failed: %s\n", SDL_GetError());
        return false;
    }

    // font
    TTF_Font *font = TTF_OpenFont("assets/bit_font.ttf", 24);
    if (font == NULL) {
        fprintf(stderr, "Font load failed: %s\n", SDL_GetError());
        return false;
    }
    a->fonts = malloc(sizeof(TTF_Font *) * 1);
    a->fonts[0] = font;

    // UI
    if (!UI_Init(WINDOW_WIDTH, WINDOW_HEIGHT, a->renderer, a->textEngine, a->fonts)) {
        fprintf(stderr, "Error initializing UI\n");
        return false;
    }

    a->running = true;
    return true;
}

void free_app(App *a) {
    if (a->fonts) {
        for (int i = 0; i < 1; i++) {
            TTF_CloseFont(a->fonts[i]);
            a->fonts[i] = NULL;
        }
        free(a->fonts);
        a->fonts = NULL;
    }
    if (a->textEngine != NULL) {
        TTF_DestroyRendererTextEngine(a->textEngine);
        a->textEngine = NULL;
    }
    if (a->renderer != NULL) {
        SDL_DestroyRenderer(a->renderer);
        a->renderer = NULL;
    }
    if (a->window != NULL) {
        SDL_DestroyWindow(a->window);
        a->window = NULL;
    }
    UI_Free();
    TTF_Quit();
    SDL_Quit();
}

void loop(App *a) {
    Enter(&(a->running));
    while (a->running) {
        Input();
        Update();
        Render(a->renderer);
    }
    Exit();
}


int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    App game = {0};

    if (!init_app(&game)) {
        free_app(&game);
        return EXIT_FAILURE;
    }
    loop(&game);
    free_app(&game);

    return EXIT_SUCCESS;
}
