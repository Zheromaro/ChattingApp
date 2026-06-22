#include <stdbool.h>
#include <SDL2/SDL.h>
#include "LoopLogic/audio.h"
#include "LoopLogic/image.h"
#include "LoopLogic/text.h"
#include "LoopLogic/appSettings.h"
#include "LoopLogic/State.h"
#include "GameConstant.h"


SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
bool gameIsRunning = false;
bool reload = false;

bool initialize_window(void){
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        fprintf(stderr, "Error initializing SDL. \n");
        return false;
    }

    window = SDL_CreateWindow(
        "Zahrawi Chess",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );
    if (!window){
        fprintf(stderr, "Error creating SDL Window. \n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL Renderer: %s\n", SDL_GetError());
        return false;
    }

    // other libraries
    AppSettingInit(&gameIsRunning, &reload);
    if (!imageInit(renderer)) return false;
    if (!textInit()) return false;
    if (!audioInit()) return false;
    if (!StateInit()) return false;

    return true;
}
void destroy_window(void){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // other libraries
    imageRelease();
    textRelease();
    audioRelease();
    AppSettingRelease();
    StateRelease();
}

int oldmain(){
    gameIsRunning = initialize_window();
    if (gameIsRunning == false) {
        destroy_window();
        return 0;
    }

    pushState(mainMenuState, "");
    while (gameIsRunning)
    {
        if (reload) ReloadState();
        reload = false;

        while (!reload && gameIsRunning)
        {
            StateUpdate();
            ProcessInput();
            Update();
            Render(renderer);
            audioUpdate();
        }
    }

    destroy_window();
    return 0;
}
