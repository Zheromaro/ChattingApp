#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include <stdbool.h>
#include <stddef.h>

typedef union SDL_Event SDL_Event;
typedef struct TextBox TextBox;

TextBox* TBCreate(void);
void TBDestroy(TextBox* tb);
void TBUpdate(TextBox* tb, float delta_time);
bool TBHandleEvent(TextBox* tb, const SDL_Event* event);

const char* TBGetText(const TextBox* tb);
void TBClear(TextBox* tb);

size_t TBGetCursorPos(const TextBox* tb);
bool TBHasSelection(const TextBox* tb);
size_t TBGetSelectionStart(const TextBox* tb);
size_t TBGetSelectionEnd(const TextBox* tb);

/* Returns true once after Return is pressed, then resets. */
bool TBShouldSend(TextBox* tb);

/* Measure pixel width of text[0 .. up_to_index). */
int TBMeasureWidth(const TextBox* tb, int font_id, size_t up_to_index);

#endif
