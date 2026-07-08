#include "Helper/ID.h"
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static int GetRandomBytes(void *buf, size_t len) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) return -1;
    ssize_t n = read(fd, buf, len);
    close(fd);
    return (n == (ssize_t)len) ? 0 : -1;
}

char* GenerateIDString(void) {
    uint8_t bytes[16];
    if (GetRandomBytes(bytes, 16) != 0) return NULL;

    // UUIDv4 variant bits
    bytes[6] = (bytes[6] & 0x0F) | 0x40;  // Version 4
    bytes[8] = (bytes[8] & 0x3F) | 0x80;  // Variant 10

    char* out = malloc(37);
    if (!out) return NULL;
    snprintf(out, 37,
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        bytes[0], bytes[1], bytes[2], bytes[3],
        bytes[4], bytes[5], bytes[6], bytes[7],
        bytes[8], bytes[9], bytes[10], bytes[11],
        bytes[12], bytes[13], bytes[14], bytes[15]);
    return out;
}
