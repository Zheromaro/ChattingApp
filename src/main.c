#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

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

    g->running = true;
    return true;
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

int main() {
    Game game = {0};

    if (!init_sdl(&game)) {
        free_sdl(&game);
        return EXIT_FAILURE;
    }

    // Main event loop
    while (game.running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                game.running = false;
            }
        }

        SDL_SetRenderDrawColor(game.renderer, 30, 30, 30, 255);
        SDL_RenderClear(game.renderer);
        SDL_RenderPresent(game.renderer);
    }

    free_sdl(&game);
    return EXIT_SUCCESS;
}
