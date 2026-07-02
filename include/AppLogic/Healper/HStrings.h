#ifndef _APPSTRINGS_H
#define _APPSTRINGS_H
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

char* string_dupstr(const char* string);
char* string_getutf8char(const char* utf8_string, size_t* index, size_t utf8_stringlength);
bool string_concatstr(char** dest_string, const char* src_string);
#endif