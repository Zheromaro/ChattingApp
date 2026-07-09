#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

#include <clay.h>

typedef struct TextBox TextBox;

void TextInputContent(TextBox* tb, const char* placeholder,
                      uint16_t fontSize, Clay_Color textColor,
                      Clay_Color placeholderColor, uint16_t fontId);

#endif
