#ifndef LOOPFUNC_H
#define LOOPFUNC_H

#include <SDL3/SDL.h>
#include <LoopLogic/SETTINGS.h>


void Enter(void);
void Exit(void);
void Input(Game *g);
void Update(void);
void Render(SDL_Renderer *renderer);

#endif
