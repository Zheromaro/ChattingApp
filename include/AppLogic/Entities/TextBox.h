#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include <stdbool.h>
#include <stddef.h>
#include <SDL3/SDL.h>

typedef struct text_buffer text_buffer_t;

text_buffer_t* tb_create(void);
void tb_destroy(text_buffer_t* tb);

/* Process SDL input events. Returns true if the event was consumed. */
bool tb_handle_event(text_buffer_t* tb, const SDL_Event* event);

/* Per-frame update (cursor blink). */
void tb_update(text_buffer_t* tb, float delta_time);

const char* tb_get_text(const text_buffer_t* tb);
void tb_clear(text_buffer_t* tb);

size_t tb_get_cursor_pos(const text_buffer_t* tb);
bool tb_has_selection(const text_buffer_t* tb);
size_t tb_get_selection_start(const text_buffer_t* tb);
size_t tb_get_selection_end(const text_buffer_t* tb);

/* Returns true once after Return is pressed, then resets. */
bool tb_should_send(text_buffer_t* tb);

/* Measure pixel width of text[0 .. up_to_index). */
int tb_measure_width(const text_buffer_t* tb, int font_id, size_t up_to_index);

#endif
