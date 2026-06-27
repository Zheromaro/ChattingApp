#ifndef CONST_UI_H
#define CONST_UI_H

#include <string.h>
#include <clay.h>

static const Clay_Color C_WHITE         = {255, 255, 255, 255};
static const Clay_Color C_BLACK         = {0, 0, 0, 255};
static const Clay_Color C_TELEGRAM_BLUE = {44, 147, 227, 255};
static const Clay_Color C_BG_GREY       = {231, 235, 240, 255};
static const Clay_Color C_INPUT_BG      = {244, 244, 245, 255};
static const Clay_Color C_PLACEHOLDER   = {142, 142, 147, 255};

static const Clay_CornerRadius RADIUS_INCOMING = {12, 12, 4, 12};
static const Clay_CornerRadius RADIUS_OUTGOING = {12, 12, 12, 4};
static const Clay_CornerRadius RADIUS_PILL     = {18, 18, 18, 18};

#define CLAY_STR(cstr) \
    (Clay_String){ .length = (int32_t)strlen(cstr), .chars = (cstr), .isStaticallyAllocated = true }

#endif
