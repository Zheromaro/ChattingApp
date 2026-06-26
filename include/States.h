#ifndef STATES_H
#define STATES_H

#include <SDL3/SDL.h>
#include <stdbool.h>

typedef void(*SEnter)(void);
typedef void(*SExit)(void);
typedef void(*SProcessInput)(SDL_Event event);
typedef void(*SUpdate)(float delta_time);
typedef void(*SRender)(SDL_Renderer *renderer);

typedef struct State {
    SEnter Enter;
    SExit Exit;
    SProcessInput Input;
    SUpdate Update;
    SRender Render;
    int priority;           // Which layer this state belongs to
} State;

typedef enum {
    LAYER_BACKGROUND = 0,
    LAYER_CONTENT    = 5,
    LAYER_MENU       = 10,
    LAYER_LOOPLOGIC  = 50,
    LAYER_MAX        = 100  // Upper bound for array sizing
} PriorityLayers;

// Initialize the state machine system
void States_Init(void);

// Add a state to its designated layer (replaces any existing state at that layer if not stacked)
// If the layer is configured as a stack, pushes onto that layer's stack
void States_Push(State *state);

// Pop top state from a specific priority layer
void States_Pop(int priority);

// Get the currently active state for a given priority layer (top of stack, or sole state)
State* States_GetActive(int priority);

// Iterate all layers in priority order, calling Input for each active state
void States_Input(SDL_Event *event);

// Iterate all layers in priority order, calling Update for each active state
void States_Update(float delta_time);

// Iterate all layers in priority order, calling Render for each active state
void States_Render(SDL_Renderer *renderer);

#endif
