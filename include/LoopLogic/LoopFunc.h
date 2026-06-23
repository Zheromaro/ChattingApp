#ifndef LOOPFUNC_H
#define LOOPFUNC_H

#include <SDL3/SDL.h>
#include <LoopLogic/SETTINGS.h>


typedef void(*SEnter)(void* contextTable[]);
typedef void(*SExit)();
typedef void(*SProcessInput)(SDL_Event event);
typedef void(*SUpdate)(float delta_time);
typedef void(*SRender)(SDL_Renderer *renderer);


typedef struct State
{
    SEnter Enter;
    SExit Exit;
    SProcessInput Input;
    SUpdate Update;
    SRender Render;
} State;

void Input(Game *g);
void Update();
void Render(SDL_Renderer *renderer);

#endif
