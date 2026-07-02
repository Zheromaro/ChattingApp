#ifndef LOOPFUNC_H
#define LOOPFUNC_H

#include <stdbool.h>
typedef struct SDL_Renderer SDL_Renderer;

void Enter(bool* running);
void Exit(void);
void Input(void);
void Update(void);
void Render(SDL_Renderer *renderer);

#endif
