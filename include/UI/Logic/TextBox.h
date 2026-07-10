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

char* TBTakeText(TextBox* tb);
void TBClear(TextBox* tb);
const char* TBGetText(TextBox* tb);
size_t TBGetByteOffset(const TextBox* tb, size_t char_index);
size_t TBGetCursorPos(const TextBox* tb);
bool TBHasSelection(const TextBox* tb);
size_t TBGetSelectionStart(const TextBox* tb);
size_t TBGetSelectionEnd(const TextBox* tb);
size_t TBGetCharCount(const TextBox* tb);
bool   TBIsCursorVisible(const TextBox* tb);

#endif
