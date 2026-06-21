#ifndef TEXT_H
#define TEXT_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

bool textInit();
void textRelease();
SDL_Texture* LoadText(const char *file);
SDL_Texture* TextTextureSolid(const char *text);
SDL_Texture* TextTextureShaded(const char *text);
SDL_Texture* TextTextureBlended(const char *text);
SDL_Texture* NumTextureBlended(const int num);

#endif