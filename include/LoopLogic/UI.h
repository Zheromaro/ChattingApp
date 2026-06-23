#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <clay.h>
#include <LoopLogic/SETTINGS.h>

// Initialize Clay UI system
// Returns false on failure
bool UI_Init(int width, int height, Game *game);

// Process SDL events for Clay input
// Call this BEFORE Clay_BeginLayout in your frame loop
void UI_Input(SDL_Event e);

// Update Clay pointer state and layout dimensions
// Call this AFTER Clay_EndLayout, before rendering
void UI_Update(void);

// Render Clay commands to the SDL renderer
void UI_Render(Clay_RenderCommandArray *commands);

// Cleanup Clay resources
void UI_Free(void);

#endif
