#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include "esp_system.h"

void save_log(char *unit, char *msg);

typedef struct {
    uint8_t *buf;
    size_t len;
    char *name;
    char *filename;
} http_post_img_t;

typedef struct {
    int tank_fluid;
    int lux;
    int moisture;
    double ph;
} sensors_data;

#endif