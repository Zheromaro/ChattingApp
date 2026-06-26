#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <clay.h>
#include <LoopLogic/SETTINGS.h>

// Initialize Clay UI system.
// Returns false on failure
bool UI_Init(int width, int height, Game *game);

// Cleanup Clay resources
void UI_Free(void);

Clay_RenderCommandArray Build_UI(float delta_time);

#endif
