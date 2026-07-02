#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <LoopLogic/Json.h>

cJSON *root = NULL;

static char *read_file(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(len + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

bool Json_Init(void) {
    char *json_text = read_file("assets/data.json");
    if (json_text == NULL) {
        root = cJSON_CreateObject();
        if (root == NULL) {
            fprintf(stderr, "Failed to create cJSON object\n");
            return false;
        }
    }
    else {
        root = cJSON_Parse(json_text);
        free(json_text);
        if (root == NULL
        ) {
            const char *err = cJSON_GetErrorPtr();
            fprintf(stderr, "Parse error before: %s\n", err ? err : "unknown");
            return false;
        }
    }

    return true;
}

void Json_Free(void) {
    // TODO: cJSON_Delete(parsed);
    cJSON_Delete(root);
}
