#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include <stdarg.h>

// TODO : Better Context Management
#define MAX_CONTEXT_NUM 10

typedef void(*PEnter)(void* contextTable[]);
typedef void(*PExit)();
typedef void(*PReloading)();
typedef void(*PProcessInput)(SDL_Event event);
typedef void(*PUpdate)(float delta_time);
typedef void(*PRender)(SDL_Renderer *renderer);

typedef struct State
{
    PEnter enter;
    PExit exit;
    PReloading reloading;
    PProcessInput processInput;
    PUpdate update;
    PRender render;
} State;

extern State mainMenuState;
extern State twoPlayerState;
extern State gameOverState;
//extern State VScpuState;
extern State choosePlayerState;

bool StateInit();
void StateUpdate(); // <- Enter & Exit
void StateRelease();

// State management
void pushState(State newState, const char* format, ...);
void popState();
void popToState(State dis);

// Game loop functions
void ReloadState();
void ProcessInput();
void Update();
void Render(SDL_Renderer *renderer);

#endif