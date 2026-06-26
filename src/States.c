#include "States.h"
#include <string.h>

#define MAX_STATES_PER_LAYER 16
#define MAX_LAYERS 128

typedef struct {
    State *stack[MAX_STATES_PER_LAYER];
    int count;
} Layer;

static Layer layers[MAX_LAYERS] = {0};

void States_Init(void) {
    memset(layers, 0, sizeof(layers));
}

void States_Push(State *state) {
    if (!state || state->priority < 0 || state->priority >= MAX_LAYERS) return;

    Layer *layer = &layers[state->priority];

    if (layer->count >= MAX_STATES_PER_LAYER) return;

    // Call Exit on current top if exists
    if (layer->count > 0 && layer->stack[layer->count - 1]->Exit) {
        layer->stack[layer->count - 1]->Exit();
    }

    layer->stack[layer->count++] = state;

    if (state->Enter) state->Enter();
}

void States_Pop(int priority) {
    if (priority < 0 || priority >= MAX_LAYERS) return;

    Layer *layer = &layers[priority];
    if (layer->count <= 0) return;

    State *top = layer->stack[layer->count - 1];
    if (top && top->Exit) top->Exit();

    layer->count--;

    // Enter the new top
    if (layer->count > 0) {
        State *newTop = layer->stack[layer->count - 1];
        if (newTop && newTop->Enter) newTop->Enter();
    }
}

State* States_GetActive(int priority) {
    if (priority < 0 || priority >= MAX_LAYERS) return NULL;
    Layer *layer = &layers[priority];
    if (layer->count <= 0) return NULL;
    return layer->stack[layer->count - 1];
}

// loopFunc
void States_Input(SDL_Event *event) {
    for (int i = 0; i < MAX_LAYERS; i++) {
        Layer *layer = &layers[i];
        if (layer->count > 0) {
            State *active = layer->stack[layer->count - 1];
            if (active && active->Input) active->Input(*event);
        }
    }
}
void States_Update(float delta_time) {
    for (int i = 0; i < MAX_LAYERS; i++) {
        Layer *layer = &layers[i];
        if (layer->count > 0) {
            State *active = layer->stack[layer->count - 1];
            if (active && active->Update) active->Update(delta_time);
        }
    }
}
void States_Render(SDL_Renderer *renderer) {
    for (int i = 0; i < MAX_LAYERS; i++) {
        Layer *layer = &layers[i];
        if (layer->count > 0) {
            State *active = layer->stack[layer->count - 1];
            if (active && active->Render) active->Render(renderer);
        }
    }
}
