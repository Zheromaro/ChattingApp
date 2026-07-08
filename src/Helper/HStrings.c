
#include "Helper/HStrings.h"
#include <stdio.h>

char* string_getutf8char_windows(const char* utf8_string, size_t* index, size_t utf8_stringlength) {
    if((*index) >= utf8_stringlength) {
        return NULL;
    }
    char* utf8_char = NULL;
    int num_bytes = 0;
    // ASCII Byte
    if((utf8_string[(*index)] & 0x80) == 0) {
        num_bytes = 1;
    } else if((utf8_string[(*index)] & 0xE0) == 0xC0) {
        num_bytes = 2;
    } else if((utf8_string[(*index)] & 0xF0) == 0xE0) {
        num_bytes = 3;
    } else if((utf8_string[(*index)] & 0xF1) == 0xF0) {
        num_bytes = 4;
    } else {
        return NULL;
    }
    utf8_char = malloc(num_bytes+1);
    if(utf8_char) {
        strncpy(utf8_char, utf8_string + (*index), num_bytes);
    }
    utf8_char[num_bytes] = '\0';
    *index += num_bytes-1;
    return utf8_char;
}

char* string_getutf8char_unixlike(const char* utf8_string, size_t* index, size_t utf8_stringlength) {
    if((*index) >= utf8_stringlength) {
        return NULL;
    }
    wchar_t wchar = L'\0';
    char* utf8_char = NULL;
    mbstate_t mb_state = {0};
    size_t num_bytes = 0;
    if((*index) < utf8_stringlength) {
        num_bytes = mbrtowc(&wchar, utf8_string+(*index), utf8_stringlength, &mb_state);
        utf8_char = malloc(num_bytes+1);
    }
    if(utf8_char) {
        strncpy(utf8_char, utf8_string+(*index), num_bytes);
        utf8_char[num_bytes] = '\0';
    }
    (*index) += num_bytes;
    return utf8_char;
}

char* string_getutf8char(const char* utf8_string, size_t* index, size_t utf8_stringlength) {
    char* utf8_char = NULL;
    #ifdef _WIN32
    utf8_char = string_getutf8char_windows(utf8_string, index, utf8_stringlength);
    #else
    utf8_char = string_getutf8char_unixlike(utf8_string, index, utf8_stringlength);
    #endif
    return utf8_char;
}

char* string_dup(const char* string) {
    if (string == NULL) return NULL;
    size_t len = strlen(string) + 1;
    char* dubstr = malloc(len);
    if (dubstr) memcpy(dubstr, string, len);
    return dubstr;
}

bool string_concatstr(char** dest_string, const char* src_string) {
    if(src_string == NULL) return false;

    size_t dest_stringlen = (*dest_string) ? strlen((*dest_string)) : 0;
    size_t src_stringlen = strlen(src_string);
    size_t new_strlen = dest_stringlen + src_stringlen;
    char* new_str = realloc((*dest_string), new_strlen + 1);

    if(!new_str) return false;
    strcpy(new_str+dest_stringlen, src_string);
    new_str[new_strlen] = '\0';
    *dest_string = new_str;
    return true;
}
