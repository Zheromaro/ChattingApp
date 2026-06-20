#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "LoopLogic/text.h"
#include "LoopLogic/image.h"
#include "GameConstant.h"


SDL_Color blackColor = {0, 0, 0};
SDL_Color whiteColor = {255, 255, 255};
TTF_Font *font = NULL;
char fontFile[] = "assets/font/rainyhearts.ttf";

bool textInit() {
    if(TTF_Init() == -1){
        printf("SDL_ttf could not be initialized! SDL_ttf Error : %s\n", TTF_GetError());
        return false;
    }

    font = TTF_OpenFont(fontFile, FONTSIZE);
    if (font == NULL){
        printf("could not load Font! SDL_ttf Error : %s\n", TTF_GetError());
        return false;
    }
    TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

    return true;
}

void textRelease(){
    TTF_CloseFont(font); // Must be before TTF_Quit();
    TTF_Quit();
}

// for High level code

// Fast but low quality
SDL_Texture* TextTextureSolid(const char *text){
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = TTF_RenderText_Solid(font, text, whiteColor);

    if(loadedSurface == NULL){
        printf("Unable to load the surface! SDL_ttf Error: %s\n", TTF_GetError());
    } else {
        newTexture = SurfaceToTexture(loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

// High quality and Fast but with a solid color Background
SDL_Texture* TextTextureShaded(const char *text){
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = TTF_RenderText_Shaded(font, text, whiteColor, blackColor);

    if(loadedSurface == NULL){
        printf("Unable to load the surface! SDL_ttf Error: %s\n", TTF_GetError());
    } else {
        newTexture = SurfaceToTexture(loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

// High quality but slow
SDL_Texture* TextTextureBlended(const char *text){
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = TTF_RenderText_Blended(font, text, whiteColor);

    if(loadedSurface == NULL){
        printf("Unable to load the surface! SDL_ttf Error: %s\n", TTF_GetError());
    } else {
        newTexture = SurfaceToTexture(loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

// High quality but slow
SDL_Texture* NumTextureBlended(const int num){
    SDL_Texture* newTexture = NULL;

    char text[10];
    sprintf(text, "%d", num);
    SDL_Surface* loadedSurface = TTF_RenderText_Blended(font, text, whiteColor);

    if(loadedSurface == NULL){
        printf("Unable to load the surface! SDL_ttf Error: %s\n", TTF_GetError());
    } else {
        newTexture = SurfaceToTexture(loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}
