#ifndef TEXT_H
#define TEXT_H

#include <stdbool.h>
typedef struct SDL_Renderer SDL_Renderer;
typedef struct TTF_TextEngine TTF_TextEngine;
typedef struct TTF_Font TTF_Font;

enum {
    FONT_ID_BODY_13  = 0,
    FONT_ID_BODY_15  = 1,
    FONT_ID_BODY_16  = 2,
    FONT_ID_TITLE_18 = 3
};
#define TOTAL_FONTS 4

bool Text_Init(SDL_Renderer *renderer, TTF_TextEngine **textEngine, TTF_Font ***fonts);
void Text_Free(void);

#endif
