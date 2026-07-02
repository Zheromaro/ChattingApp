#include "AppLogic/Entities/TextBox.h"
#include "AppLogic/Healper/HStrings.h"
#include <SDL3/SDL_keycode.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char* utf8_char;   /* owned */
    size_t utf8_len;
} tb_char_t;

struct text_buffer {
    tb_char_t* chars;
    size_t char_count;
    size_t cursor_pos;

    size_t sel_start;
    size_t sel_end;
    size_t sel_count;
    int sel_direction; /* 0 = left, 1 = right */

    float cursor_blink_timer;
    float cursor_blink_interval;
    bool cursor_visible;
    bool enter_pressed;
};

text_buffer_t* tb_create(void) {
    text_buffer_t* tb = calloc(1, sizeof(text_buffer_t));
    tb->cursor_blink_interval = 0.53f;
    tb->cursor_visible = true;
    return tb;
}

void tb_destroy(text_buffer_t* tb) {
    if (!tb) return;
    for (size_t i = 0; i < tb->char_count; i++) free(tb->chars[i].utf8_char);
    free(tb->chars);
    free(tb);
}

static void tb_reset_blink(text_buffer_t* tb) {
    tb->cursor_visible = true;
    tb->cursor_blink_timer = 0.0f;
}

static void tb_clear_selection(text_buffer_t* tb) {
    tb->sel_start = tb->sel_end = tb->sel_count = 0;
}

static void tb_select_all(text_buffer_t* tb) {
    tb->sel_start = 0;
    tb->sel_end = tb->char_count;
    tb->sel_count = tb->char_count;
    tb->sel_direction = 1;
    tb->cursor_pos = tb->char_count;
}

static char* tb_get_selection_text(const text_buffer_t* tb) {
    if (!tb->sel_count) return NULL;
    char* out = NULL;
    for (size_t i = tb->sel_start; i < tb->sel_end; i++)
        string_concatstr(&out, tb->chars[i].utf8_char);
    return out;
}

static void tb_delete_selection(text_buffer_t* tb) {
    if (!tb->sel_count) return;
    size_t new_count = tb->char_count - tb->sel_count;
    size_t dst = tb->sel_start;
    for (size_t i = tb->sel_end; i < tb->char_count; i++) {
        tb->chars[dst++] = tb->chars[i];
    }
    for (size_t i = new_count; i < tb->char_count; i++) free(tb->chars[i].utf8_char);
    tb->char_count = new_count;
    tb->cursor_pos = tb->sel_start;
    tb_clear_selection(tb);
    if (!new_count) {
        free(tb->chars);
        tb->chars = NULL;
    } else {
        tb_char_t* tmp = realloc(tb->chars, new_count * sizeof(tb_char_t));
        if (tmp) tb->chars = tmp;
    }
}

static void tb_backspace(text_buffer_t* tb) {
    if (!tb->char_count || !tb->cursor_pos) return;
    size_t idx = tb->cursor_pos - 1;
    free(tb->chars[idx].utf8_char);
    for (size_t i = idx; i + 1 < tb->char_count; i++)
        tb->chars[i] = tb->chars[i + 1];
    tb->char_count--;
    tb->cursor_pos--;
    if (!tb->char_count) {
        free(tb->chars);
        tb->chars = NULL;
    } else {
        tb_char_t* tmp = realloc(tb->chars, tb->char_count * sizeof(tb_char_t));
        if (tmp) tb->chars = tmp;
    }
}

static void tb_delete_at(text_buffer_t* tb, size_t pos) {
    if (pos >= tb->char_count) return;
    free(tb->chars[pos].utf8_char);
    for (size_t i = pos; i + 1 < tb->char_count; i++)
        tb->chars[i] = tb->chars[i + 1];
    tb->char_count--;
    if (!tb->char_count) {
        free(tb->chars);
        tb->chars = NULL;
    } else {
        tb_char_t* tmp = realloc(tb->chars, tb->char_count * sizeof(tb_char_t));
        if (tmp) tb->chars = tmp;
    }
}

static void tb_insert_char(text_buffer_t* tb, char* ch) {
    size_t new_count = tb->char_count + 1;
    tb_char_t* tmp = realloc(tb->chars, new_count * sizeof(tb_char_t));
    if (!tmp) { free(ch); return; }
    tb->chars = tmp;
    for (size_t i = new_count - 1; i > tb->cursor_pos; i--)
        tb->chars[i] = tb->chars[i - 1];
    tb->chars[tb->cursor_pos].utf8_char = ch;
    tb->chars[tb->cursor_pos].utf8_len = strlen(ch);
    tb->cursor_pos++;
    tb->char_count = new_count;
}

static void tb_add_selection_left(text_buffer_t* tb) {
    if (!tb->cursor_pos) return;
    if (tb->sel_direction == 1 && tb->sel_count) {
        tb->cursor_pos--;
        tb->sel_end--;
        tb->sel_count--;
        if (!tb->sel_count) tb_clear_selection(tb);
        return;
    }
    tb->sel_direction = 0;
    if (!tb->sel_count) tb->sel_end = tb->cursor_pos;
    tb->cursor_pos--;
    tb->sel_start = tb->cursor_pos;
    tb->sel_count++;
}

static void tb_add_selection_right(text_buffer_t* tb) {
    if (tb->cursor_pos >= tb->char_count) return;
    if (tb->sel_direction == 0 && tb->sel_count) {
        tb->cursor_pos++;
        tb->sel_start++;
        tb->sel_count--;
        if (!tb->sel_count) tb_clear_selection(tb);
        return;
    }
    tb->sel_direction = 1;
    if (!tb->sel_count) tb->sel_start = tb->cursor_pos;
    tb->cursor_pos++;
    tb->sel_end = tb->cursor_pos;
    tb->sel_count++;
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */

void tb_update(text_buffer_t* tb, float delta_time) {
    if (!tb) return;
    tb->cursor_blink_timer += delta_time;
    if (tb->cursor_blink_timer >= tb->cursor_blink_interval) {
        tb->cursor_blink_timer = 0.0f;
        tb->cursor_visible = !tb->cursor_visible;
    }
}

bool tb_handle_event(text_buffer_t* tb, const SDL_Event* ev) {
    if (!tb || !ev) return false;

    switch (ev->type) {
    case SDL_EVENT_TEXT_INPUT: {
        tb_delete_selection(tb);
        size_t len = strlen(ev->text.text);
        size_t idx = 0;
        while (idx < len) {
            char* ch = string_getutf8char(ev->text.text, &idx, len);
            if (ch) tb_insert_char(tb, ch);
            idx++;
        }
        tb_reset_blink(tb);
        return true;
    }

    case SDL_EVENT_KEY_DOWN: {
        SDL_Keycode key = ev->key.key;
        bool shift = (SDL_GetModState() & SDL_KMOD_SHIFT);
        bool ctrl  = (SDL_GetModState() & SDL_KMOD_CTRL);

        if (ctrl) {
            switch (key) {
            case SDLK_A:
                tb_select_all(tb);
                tb_reset_blink(tb);
                return true;
            case SDLK_C: {
                char* sel = tb_get_selection_text(tb);
                if (sel) SDL_SetClipboardText(sel);
                free(sel);
                return true;
            }
            case SDLK_V: {
                char* clip = SDL_GetClipboardText();
                if (clip) {
                    tb_delete_selection(tb);
                    size_t len = strlen(clip);
                    size_t idx = 0;
                    while (idx < len) {
                        char* ch = string_getutf8char(clip, &idx, len);
                        if (ch) tb_insert_char(tb, ch);
                        idx++;
                    }
                    SDL_free(clip);
                }
                tb_reset_blink(tb);
                return true;
            }
            }
        }

        if (shift) {
            switch (key) {
            case SDLK_LEFT:  tb_add_selection_left(tb);  tb_reset_blink(tb); return true;
            case SDLK_RIGHT: tb_add_selection_right(tb); tb_reset_blink(tb); return true;
            }
        }

        switch (key) {
        case SDLK_LEFT:
            if (tb->sel_count) {
                tb->cursor_pos = tb->sel_start;
                tb_clear_selection(tb);
            } else if (tb->cursor_pos > 0) {
                tb->cursor_pos--;
            }
            tb_reset_blink(tb);
            return true;

        case SDLK_RIGHT:
            if (tb->sel_count) {
                tb->cursor_pos = tb->sel_end;
                tb_clear_selection(tb);
            } else if (tb->cursor_pos < tb->char_count) {
                tb->cursor_pos++;
            }
            tb_reset_blink(tb);
            return true;

        case SDLK_BACKSPACE:
            if (tb->sel_count) tb_delete_selection(tb);
            else tb_backspace(tb);
            tb_reset_blink(tb);
            return true;

        case SDLK_DELETE:
            if (tb->sel_count) tb_delete_selection(tb);
            else tb_delete_at(tb, tb->cursor_pos);
            tb_reset_blink(tb);
            return true;

        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            tb->enter_pressed = true;
            return true;
        }
        break;
    }

    case SDL_EVENT_KEY_UP:
        if (ev->key.key == SDLK_RETURN || ev->key.key == SDLK_KP_ENTER)
            tb->enter_pressed = true;
        break;
    }
    return false;
}

const char* tb_get_text(const text_buffer_t* tb) {
    if (!tb || !tb->char_count) return "";
    static char* cache = NULL;
    free(cache);
    cache = NULL;
    for (size_t i = 0; i < tb->char_count; i++)
        string_concatstr(&cache, tb->chars[i].utf8_char);
    return cache ? cache : "";
}

void tb_clear(text_buffer_t* tb) {
    if (!tb) return;
    for (size_t i = 0; i < tb->char_count; i++) free(tb->chars[i].utf8_char);
    free(tb->chars);
    tb->chars = NULL;
    tb->char_count = 0;
    tb->cursor_pos = 0;
    tb_clear_selection(tb);
    tb->enter_pressed = false;
    tb_reset_blink(tb);
}

size_t tb_get_cursor_pos(const text_buffer_t* tb) {
    return tb ? tb->cursor_pos : 0;
}

bool tb_has_selection(const text_buffer_t* tb) {
    return tb && tb->sel_count > 0;
}

size_t tb_get_selection_start(const text_buffer_t* tb) {
    return tb ? tb->sel_start : 0;
}

size_t tb_get_selection_end(const text_buffer_t* tb) {
    return tb ? tb->sel_end : 0;
}

bool tb_should_send(text_buffer_t* tb) {
    if (!tb) return false;
    bool v = tb->enter_pressed;
    tb->enter_pressed = false;
    return v;
}

int tb_measure_width(const text_buffer_t* tb, int font_id, size_t up_to_index) {
    (void)font_id; /* if you need to measure via TTF, do it in MainPanel_RenderExtras */
    if (!tb || !up_to_index) return 0;
    int w = 0;
    for (size_t i = 0; i < up_to_index && i < tb->char_count; i++)
        w += (int)tb->chars[i].utf8_len * 8; /* fallback rough width */
    return w;
}
