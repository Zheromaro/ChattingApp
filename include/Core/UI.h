#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <clay.h>

// Initialize Clay UI system.
// Returns false on failure
bool UI_Init(int width, int height, SDL_Renderer *renderer, TTF_TextEngine *textEngine, TTF_Font **fonts);

// Cleanup Clay resources
void UI_Free(void);

void UI_Layout(Clay_RenderCommandArray command);
void UI_Input(SDL_Event* e);
void UI_Update(float deltaTime);
void UI_Render(void);

float UI_GetMouseX(void);
float UI_GetMouseY(void);
bool UI_GetMouseDown(void);

// TODO: Error functions

#endif
