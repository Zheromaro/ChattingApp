#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "clay_renderer_SDL3.c"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


#define SDL_FLAGS SDL_INIT_VIDEO
#define WINDOW_TITLE "Zahrawi"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct Game {
    SDL_Window *window;
    SDL_Renderer *renderer;
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

    g->running = true;
    return true;
}

void loop(Game *g){
    while (g->running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                g->running = false;
            }
        }

        SDL_SetRenderDrawColor(g->renderer, 30, 30, 30, 255);
        SDL_RenderClear(g->renderer);
        SDL_RenderPresent(g->renderer);
    }
}

void free_sdl(Game *g) {
    if (g->renderer != NULL) {
        SDL_DestroyRenderer(g->renderer);
        g->renderer = NULL;
    }
    if (g->window != NULL) {
        SDL_DestroyWindow(g->window);
        g->window = NULL;
    }
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
