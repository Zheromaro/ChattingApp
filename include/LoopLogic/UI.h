#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <clay.h>
#include "SETTINGS.h"


// Initialize Clay UI system.
// Returns false on failure
bool UI_Init(int width, int height, Game *game);

// Cleanup Clay resources
void UI_Free(void);

void UI_Layout(Clay_RenderCommandArray command);
void UI_Input(SDL_Event* e);
void UI_Update(float deltaTime);
void UI_Render();

float UI_GetMouseX();
float UI_GetMouseY();
bool UI_GetMouseDown();

// TODO: Error functions

#endif
