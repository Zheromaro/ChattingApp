#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL3/SDL.h>
#include "UI/TextBox.h"
#include "Helper/HStrings.h"

typedef struct {
    char* utf8_char;
    size_t utf8_len;
} TextBuffer;

struct TextBox {
    TextBuffer* chars;
    size_t char_count;
    size_t cursor_pos;

    size_t sel_start;
    size_t sel_end;
    size_t sel_count;
    int sel_direction; /* 0 = left, 1 = right */

    float cursor_blink_timer;
    float cursor_blink_interval;
    bool cursor_visible;
};

static void TBResetBlink(TextBox* tb) {
    tb->cursor_visible = true;
    tb->cursor_blink_timer = 0.0f;
}

static void TBClearSelection(TextBox* tb) {
    tb->sel_start = tb->sel_end = tb->sel_count = 0;
}

static void TBSelectAll(TextBox* tb) {
    tb->sel_start = 0;
    tb->sel_end = tb->char_count;
    tb->sel_count = tb->char_count;
    tb->sel_direction = 1;
    tb->cursor_pos = tb->char_count;
}

static char* TBGetSelectionText(const TextBox* tb) {
    if (!tb->sel_count) return NULL;
    char* out = NULL;
    for (size_t i = tb->sel_start; i < tb->sel_end; i++)
        string_concatstr(&out, tb->chars[i].utf8_char);
    return out;
}

static void TBDeleteSelection(TextBox* tb) {
    if (!tb->sel_count) return;
    /* 1. Free the strings that are actually being deleted */
    for (size_t i = tb->sel_start; i < tb->sel_end; i++) free(tb->chars[i].utf8_char);

    /* 2. Shift the remaining TextBuffer structs down */
    size_t new_count = tb->char_count - tb->sel_count;
    size_t dst = tb->sel_start;
    for (size_t i = tb->sel_end; i < tb->char_count; i++) {
        tb->chars[dst++] = tb->chars[i];
    }

    /* 3. Update metadata */
    tb->char_count = new_count;
    tb->cursor_pos = tb->sel_start;
    TBClearSelection(tb);

    /* 4. Shrink or free the array */
    if (!new_count) {
        free(tb->chars);
        tb->chars = NULL;
    } else {
        TextBuffer* tmp = realloc(tb->chars, new_count * sizeof(TextBuffer));
        if (tmp) tb->chars = tmp;
    }
}

static void TBBackspace(TextBox* tb) {
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
        TextBuffer* tmp = realloc(tb->chars, tb->char_count * sizeof(TextBuffer));
        if (tmp) tb->chars = tmp;
    }
}

static void TBDeleteAt(TextBox* tb, size_t pos) {
    if (pos >= tb->char_count) return;
    free(tb->chars[pos].utf8_char);
    for (size_t i = pos; i + 1 < tb->char_count; i++)
        tb->chars[i] = tb->chars[i + 1];
    tb->char_count--;
    if (!tb->char_count) {
        free(tb->chars);
        tb->chars = NULL;
    } else {
        TextBuffer* tmp = realloc(tb->chars, tb->char_count * sizeof(TextBuffer));
        if (tmp) tb->chars = tmp;
    }
}

static void TBInsertChar(TextBox* tb, char* ch) {
    size_t new_count = tb->char_count + 1;
    TextBuffer* tmp = realloc(tb->chars, new_count * sizeof(TextBuffer));
    if (!tmp) { free(ch); return; }
    tb->chars = tmp;
    for (size_t i = new_count - 1; i > tb->cursor_pos; i--)
        tb->chars[i] = tb->chars[i - 1];
    tb->chars[tb->cursor_pos].utf8_char = ch;
    tb->chars[tb->cursor_pos].utf8_len = strlen(ch);
    tb->cursor_pos++;
    tb->char_count = new_count;
}

static void TBAddSelectionLeft(TextBox* tb) {
    if (!tb->cursor_pos) return;
    if (tb->sel_direction == 1 && tb->sel_count) {
        tb->cursor_pos--;
        tb->sel_end--;
        tb->sel_count--;
        if (!tb->sel_count) TBClearSelection(tb);
        return;
    }
    tb->sel_direction = 0;
    if (!tb->sel_count) tb->sel_end = tb->cursor_pos;
    tb->cursor_pos--;
    tb->sel_start = tb->cursor_pos;
    tb->sel_count++;
}

static void TBAddSelectionRight(TextBox* tb) {
    if (tb->cursor_pos >= tb->char_count) return;
    if (tb->sel_direction == 0 && tb->sel_count) {
        tb->cursor_pos++;
        tb->sel_start++;
        tb->sel_count--;
        if (!tb->sel_count) TBClearSelection(tb);
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

TextBox* TBCreate(void) {
    TextBox* tb = calloc(1, sizeof(TextBox));
    tb->cursor_blink_interval = 0.53f;
    tb->cursor_visible = true;
    return tb;
}

void TBDestroy(TextBox* tb) {
    if (!tb) return;
    for (size_t i = 0; i < tb->char_count; i++) free(tb->chars[i].utf8_char);
    free(tb->chars);
    free(tb);
}

void TBUpdate(TextBox* tb, float delta_time) {
    if (!tb) return;
    tb->cursor_blink_timer += delta_time;
    if (tb->cursor_blink_timer >= tb->cursor_blink_interval) {
        tb->cursor_blink_timer = 0.0f;
        tb->cursor_visible = !tb->cursor_visible;
    }
}

bool TBHandleEvent(TextBox* tb, const SDL_Event* ev) {
    if (!tb || !ev) return false;

    switch (ev->type) {
    case SDL_EVENT_TEXT_INPUT: {
        TBDeleteSelection(tb);
        size_t len = strlen(ev->text.text);
        size_t idx = 0;
        while (idx < len) {
            char* ch = string_getutf8char(ev->text.text, &idx, len);
            if (ch) TBInsertChar(tb, ch);
            idx++;
        }
        TBResetBlink(tb);
        return true;
    }

    case SDL_EVENT_KEY_DOWN: {
        SDL_Keycode key = ev->key.key;
        bool shift = (SDL_GetModState() & SDL_KMOD_SHIFT);
        bool ctrl  = (SDL_GetModState() & SDL_KMOD_CTRL);

        if (ctrl) {
            switch (key) {
            case SDLK_A:
                TBSelectAll(tb);
                TBResetBlink(tb);
                return true;
            case SDLK_C: {
                char* sel = TBGetSelectionText(tb);
                if (sel) SDL_SetClipboardText(sel);
                free(sel);
                return true;
            }
            case SDLK_V: {
                char* clip = SDL_GetClipboardText();
                if (clip) {
                    TBDeleteSelection(tb);
                    size_t len = strlen(clip);
                    size_t idx = 0;
                    while (idx < len) {
                        char* ch = string_getutf8char(clip, &idx, len);
                        if (ch) TBInsertChar(tb, ch);
                        idx++;
                    }
                    SDL_free(clip);
                }
                TBResetBlink(tb);
                return true;
            }
            }
        }

        if (shift) {
            switch (key) {
            case SDLK_LEFT:  TBAddSelectionLeft(tb);  TBResetBlink(tb); return true;
            case SDLK_RIGHT: TBAddSelectionRight(tb); TBResetBlink(tb); return true;
            }
        }

        switch (key) {
        case SDLK_LEFT:
            if (tb->sel_count) {
                tb->cursor_pos = tb->sel_start;
                TBClearSelection(tb);
            } else if (tb->cursor_pos > 0) {
                tb->cursor_pos--;
            }
            TBResetBlink(tb);
            return true;

        case SDLK_RIGHT:
            if (tb->sel_count) {
                tb->cursor_pos = tb->sel_end;
                TBClearSelection(tb);
            } else if (tb->cursor_pos < tb->char_count) {
                tb->cursor_pos++;
            }
            TBResetBlink(tb);
            return true;

        case SDLK_BACKSPACE:
            if (tb->sel_count) TBDeleteSelection(tb);
            else TBBackspace(tb);
            TBResetBlink(tb);
            return true;

        case SDLK_DELETE:
            if (tb->sel_count) TBDeleteSelection(tb);
            else TBDeleteAt(tb, tb->cursor_pos);
            TBResetBlink(tb);
            return true;
        }
        break;
    }
    }
    return false;
}

const char* TBTakeText(TextBox* tb) {
    if (!tb || !tb->char_count) return "";
    static char* cache = NULL;
    free(cache);
    cache = NULL;
    for (size_t i = 0; i < tb->char_count; i++)
        string_concatstr(&cache, tb->chars[i].utf8_char);
    TBClear(tb);
    return cache ? cache : "";
}

void TBClear(TextBox* tb) {
    if (!tb) return;
    for (size_t i = 0; i < tb->char_count; i++) free(tb->chars[i].utf8_char);
    free(tb->chars);
    tb->chars = NULL;
    tb->char_count = 0;
    tb->cursor_pos = 0;
    TBClearSelection(tb);
    TBResetBlink(tb);
}

char* TBGetText(TextBox* tb) {
    if (!tb || !tb->char_count) return "";
    static char* cache = NULL;
    free(cache);
    cache = NULL;
    for (size_t i = 0; i < tb->char_count; i++)
        string_concatstr(&cache, tb->chars[i].utf8_char);
    return cache ? cache : "";
}

size_t TBGetByteOffset(const TextBox* tb, size_t char_index) {
    if (!tb) return 0;
    size_t off = 0;
    for (size_t i = 0; i < char_index && i < tb->char_count; i++)
        off += tb->chars[i].utf8_len;
    return off;
}

size_t TBGetCursorPos(const TextBox* tb) { return tb ? tb->cursor_pos : 0; }

bool TBHasSelection(const TextBox* tb) { return tb && tb->sel_count > 0; }

size_t TBGetSelectionStart(const TextBox* tb) { return tb ? tb->sel_start : 0; }

size_t TBGetSelectionEnd(const TextBox* tb) { return tb ? tb->sel_end : 0; }

size_t TBGetCharCount(const TextBox* tb) { return tb ? tb->char_count : 0; }

bool   TBIsCursorVisible(const TextBox* tb) { return tb ? tb->cursor_visible : false; }
