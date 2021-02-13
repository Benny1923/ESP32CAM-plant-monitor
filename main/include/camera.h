#ifndef CAMERA_H
#define CAMERA_H
#include "esp_system.h"
#include <stdio.h>

esp_err_t init_camera();
size_t take_picture(uint8_t **buf);

#endif