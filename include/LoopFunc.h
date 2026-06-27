#ifndef LOOPFUNC_H
#define LOOPFUNC_H

#include <SDL3/SDL.h>

void Enter(bool* running);
void Exit(void);
void Input(void);
void Update(void);
void Render(SDL_Renderer *renderer);

#endif
