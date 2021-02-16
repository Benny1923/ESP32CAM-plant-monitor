#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include "esp_system.h"

typedef struct {
    uint8_t *buf;
    size_t len;
    char *name;
    char *filename;
} http_post_img_t;

#endif