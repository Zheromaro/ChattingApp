#ifndef CONST_UI_H
#define CONST_UI_H

#include <string.h>
#include <stdio.h>
#include <clay.h>

enum {
    FONT_ID_BODY_13  = 0,
    FONT_ID_BODY_15  = 1,
    FONT_ID_BODY_16  = 2,
    FONT_ID_TITLE_18 = 3
};
#define TOTAL_FONTS 4

static const Clay_Color C_WHITE         = {255, 255, 255, 255};
static const Clay_Color C_BLACK         = {0, 0, 0, 255};
static const Clay_Color C_BLUE = {44, 147, 227, 255};
static const Clay_Color C_BG_GREY       = {231, 235, 240, 255};
static const Clay_Color C_INPUT_BG      = {244, 244, 245, 255};
static const Clay_Color C_PLACEHOLDER   = {142, 142, 147, 255};

static const Clay_CornerRadius RADIUS_INCOMING = {12, 12, 4, 12};
static const Clay_CornerRadius RADIUS_OUTGOING = {12, 12, 12, 4};
static const Clay_CornerRadius RADIUS_PILL     = {18, 18, 18, 18};

#define CLAY_STR(cstr) \
    (Clay_String){ .length = (cstr) ? (int32_t)strlen(cstr) : 0, .chars = (cstr) ? (cstr) : "", .isStaticallyAllocated = true }

static Clay_Color HexColor(const char *hex) {
    unsigned int r, g, b;
    sscanf(hex, "%02x%02x%02x", &r, &g, &b);
    return (Clay_Color){ (float)r, (float)g, (float)b, 255 };
}

#endif
