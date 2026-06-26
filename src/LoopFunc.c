#include <SDL3/SDL.h>
#include "LoopFunc.h"
#include "States.h"

static Uint64 last_tick = 0;
static Uint64 now = 0;
static Uint64 frame_time = 0;
static Uint64 target_ns = 1000000000ULL / 60;
static float delta_time = 0;
static SDL_Event event;

void Enter(void) {
    last_tick = SDL_GetTicksNS();
}

void Exit(void) {

}

void Input(Game *g) {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                g->running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE)
                    g->running = false;
                break;
        }
        States_Input(&event);
    }
}

void Update(void) {
    now = SDL_GetTicksNS();
    delta_time = (now - last_tick) / 1000000000.0f;
    last_tick = now;

    if (delta_time > 0.25f) delta_time = 0.25f;

    States_Update(delta_time);

    // Frame cap
    frame_time = SDL_GetTicksNS() - now;
    if (frame_time < target_ns) {
        SDL_DelayNS(target_ns - frame_time);
    }
}

void Render(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    States_Render(renderer);

    SDL_RenderPresent(renderer);
}
