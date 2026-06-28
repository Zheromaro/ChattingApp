#include <SDL3/SDL.h>
#include <stdbool.h>
#include "LoopFunc.h"
#include "LoopLogic/UI.h"
#include "AppLogic/Chat.h"


static Uint64 last_tick = 0;
static Uint64 now = 0;
static Uint64 frame_time = 0;
static Uint64 target_ns = 1000000000ULL / 60;
static float delta_time = 0;
static SDL_Event event;
static bool* prunning = NULL;

void Enter(bool* running) {
    prunning = running;
    last_tick = SDL_GetTicksNS();
}

void Exit(void) {

}

void Input(void) {
    while (SDL_PollEvent(&event)) {
        UI_Input(&event);
        switch (event.type) {
            case SDL_EVENT_QUIT:
                *prunning = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE)
                    *prunning = false;
                break;
        }
    }
}

void Update(void) {
    now = SDL_GetTicksNS();
    delta_time = (now - last_tick) / 1000000000.0f;
    last_tick = now;

    if (delta_time > 0.25f) delta_time = 0.25f;

    // update functions
    UI_Update(delta_time);
    ChatUpdate(delta_time);
}

void Render(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);
    UI_Render();
    SDL_RenderPresent(renderer);
}
