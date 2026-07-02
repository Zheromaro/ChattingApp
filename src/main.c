#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "LoopLogic/Json.h"
#include "LoopLogic/LoopFunc.h"
#include "LoopLogic/Renderer.h"
#include "LoopLogic/Text.h"
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
    SDL_StartTextInput(a->window);

    if (!Renderer_Init(&a->renderer, a->window)) {
        fprintf(stderr, "Error Creating Renderer.\n");
        return false;
    }

    if (!Text_Init(a->renderer, &a->textEngine, &a->fonts)) {
        fprintf(stderr, "Error initializing text.\n");
        return false;
    }

    if (!UI_Init(WINDOW_WIDTH, WINDOW_HEIGHT, a->renderer, a->textEngine, a->fonts)) {
        fprintf(stderr, "Error initializing UI.\n");
        return false;
    }

    a->running = true;
    return true;
}

void free_app(App *a) {
    UI_Free();
    Text_Free();
    SDL_StopTextInput(a->window);
    if (a->window != NULL) {
        SDL_DestroyWindow(a->window);
        a->window = NULL;
    }
    Renderer_Free();
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
