#ifndef SDCARD_H
#define SDCARD_H
#include <stdio.h>

void init_sdcard();
void save_file(uint8_t *buf, size_t len, char *filename);

#endif