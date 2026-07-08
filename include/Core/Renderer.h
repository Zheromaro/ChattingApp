#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Window SDL_Window;

bool Renderer_Init(SDL_Renderer **renderer, SDL_Window *window);
void Renderer_Free(void);

#endif
