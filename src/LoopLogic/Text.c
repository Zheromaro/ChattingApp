#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "LoopLogic/Text.h"

static TTF_TextEngine *textEngine;
static TTF_Font **fonts;

bool Text_Init(SDL_Renderer *renderer, TTF_TextEngine **ptextEngine, TTF_Font ***pfonts)
{
    if (!TTF_Init()) {
        fprintf(stderr, "Error initializing SDL_ttf: %s\n", SDL_GetError());
        return false;
    }

    textEngine = TTF_CreateRendererTextEngine(renderer);
    if (textEngine == NULL) {
        fprintf(stderr, "Text engine failed: %s\n", SDL_GetError());
        return false;
    }

    // fonts
    fonts = malloc(sizeof(TTF_Font *) * TOTAL_FONTS);
    if (fonts == NULL) {
        fprintf(stderr, "Failed to allocate memory for fonts array\n");
        return false;
    }
    for(int i = 0; i < TOTAL_FONTS; i++) {
        fonts[i] = NULL;
    }

    const char* fontPath = "assets/Ubuntu-Medium.ttf";
    fonts[FONT_ID_BODY_13] = TTF_OpenFont(fontPath, 13);
    fonts[FONT_ID_BODY_15] = TTF_OpenFont(fontPath, 15);
    fonts[FONT_ID_BODY_16] = TTF_OpenFont(fontPath, 16);
    fonts[FONT_ID_TITLE_18]  = TTF_OpenFont(fontPath, 18);
    for (int i = 0; i < TOTAL_FONTS; i++) {
        if (fonts[i] == NULL) {
            fprintf(stderr, "Font load failed for index %d: %s\n", i, SDL_GetError());
            return false;
        }
    }

    *ptextEngine = textEngine;
    *pfonts = fonts;
    return true;
}

void Text_Free(void) {
    if (fonts) {
        for (int i = 0; i < TOTAL_FONTS; i++) {
            TTF_CloseFont(fonts[i]);
            fonts[i] = NULL;
        }
        free(fonts);
        fonts = NULL;
    }
    if (textEngine != NULL) {
        TTF_DestroyRendererTextEngine(textEngine);
        textEngine = NULL;
    }
    TTF_Quit();
}
