#ifndef CONST_UI_H
#define CONST_UI_H

#include <stdio.h>
#include <stdint.h>
#include <clay.h>

/* ── Colors ─────────────────────────────────────────────────────── */
static const Clay_Color C_WHITE       = {255, 255, 255, 255};
static const Clay_Color C_BLACK       = {0,   0,   0,   255};
static const Clay_Color C_BLUE        = {44,  147, 227, 255};
static const Clay_Color C_BG_GREY     = {231, 235, 240, 255};
static const Clay_Color C_INPUT_BG    = {244, 244, 245, 255};
static const Clay_Color C_PLACEHOLDER = {142, 142, 147, 255};

/* ── Radii ──────────────────────────────────────────────────────── */
static const Clay_CornerRadius RADIUS_INCOMING = {12, 12, 4,  12};
static const Clay_CornerRadius RADIUS_OUTGOING = {12, 12, 12, 4};
static const Clay_CornerRadius RADIUS_PILL      = {18, 18, 18, 18};

/* ── Helpers ────────────────────────────────────────────────────── */

/**
 * Wrap a C string into a Clay_String.
 * The string MUST remain valid for the duration of the layout pass.
 * For dynamic / heap strings, use CLAY_SIDI() or manage lifetime carefully.
 */
#define CLAY_STR(cstr) \
    ((Clay_String){ \
        .length = (int32_t)strlen(cstr), \
        .chars   = (cstr), \
        .isStaticallyAllocated = false \
    })

/**
 * Parse a hex color string (e.g. "FF5733") into Clay_Color.
 * Channels are 0-255, alpha is always 255.
 */
static inline Clay_Color HexColor(const char *hex)
{
    unsigned int r = 0, g = 0, b = 0;
    sscanf(hex, "%02x%02x%02x", &r, &g, &b);
    return (Clay_Color){ (float)r, (float)g, (float)b, 255.0f };
}

#endif
