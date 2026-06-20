#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "LoopLogic/image.h"

SDL_Renderer *imageRenderer = NULL;

bool imageInit(SDL_Renderer *renderer) {
    imageRenderer = renderer;
    
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if(!(IMG_Init(imgFlags) & imgFlags)){
        printf("SDL_image could not be initialized! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }

    return true;
}
void imageRelease(){
    IMG_Quit();
}

// for High level code

// DON'T do it in update
SDL_Texture* LoadTexture(const char *file){
    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(file);
    if(loadedSurface == NULL){
        printf("Unable to load the image %s! SDL_image Error: %s\n", file, SDL_GetError());
    } else {
        newTexture = SDL_CreateTextureFromSurface(imageRenderer, loadedSurface);

        if(newTexture == NULL)
            printf("Unable to texture from %s! SDL_image Error: %s\n", file, SDL_GetError());

        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}
SDL_Texture* SurfaceToTexture(SDL_Surface *surface){
    SDL_Texture* newTexture = NULL;

    if(surface == NULL){
        printf("the given surface pointer is null");
    } else {
        newTexture = SDL_CreateTextureFromSurface(imageRenderer, surface);

        if(newTexture == NULL)
            printf("Unable to texture from the given surface pointer");
    }

    return newTexture;
}