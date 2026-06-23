#include <SDL3/SDL.h>
#include <clay.h>
#include "LoopLogic/LoopFunc.h"


// Update variables
Uint64 last_tick = 0;
Uint64 now = 0;
Uint64 frame_time = 0;
Uint64 target_ns = 1000000000ULL / 60;   // 60 FPS cap (optional)
float delta_time = 0;
// UI variables
Clay_RenderCommandArray *ui_commands;
// State
State* current = NULL;
// Input
SDL_Event event;


void Input(Game *g)
{
    while (SDL_PollEvent(&event)) {
        if (current && current->Input)
            ((SProcessInput)current->Input)(event);
        switch (event.type) {
            case SDL_EVENT_QUIT:
                g->running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE)
                    g->running = false;
                break;
        }
    }
}
void Update()
{
    now = SDL_GetTicksNS();
    delta_time = (now - last_tick) / 1000000000.0f;  /* seconds */
    last_tick = now;

    // safety clamp: if we hit a breakpoint dt doesn't explode
    if (delta_time > 0.25f) delta_time = 0.25f;

    if (current && current->Update)
        ((SUpdate)current->Update)(delta_time);

    // frame cap
    frame_time = SDL_GetTicksNS() - now;
    if (frame_time < target_ns) {
        SDL_DelayNS(target_ns - frame_time);
    }
}
void Render(SDL_Renderer *renderer)
{
    // clear background
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    // draw here
    if (current && current->Render)
        ((SRender)current->Render)(renderer);

    // push to screen
    SDL_RenderPresent(renderer);
}
