#include <stdio.h>
#include <SDL3/SDL.h>
#include "Core/Renderer.h"

SDL_Renderer *renderer;

bool Renderer_Init(SDL_Renderer **prenderer, SDL_Window *window) {
    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        fprintf(stderr, "Error Creating Renderer: %s\n", SDL_GetError());
        return false;
    }
    SDL_SetRenderVSync(renderer, 1);

    *prenderer = renderer;
    return true;
}

void Renderer_Free(void) {
    if (renderer != NULL) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
}
