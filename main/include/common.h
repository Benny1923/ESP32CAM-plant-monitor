#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include "esp_system.h"

void ntp_adj(void);
void save_log(char *unit, char *msg);
esp_err_t load_config(void);
char *create_response_msg(int type, void *ptr);

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
} sensors_data_t;

typedef struct {
    char *start;
    char *end;
    int max;
    int min;
} light_config_t;

typedef struct {
    char *start;
    char *end;
    int min;
} sprinklers_config_t;

typedef struct {
    char *start;
    char *end;
    int interval;
    int nightmode;
} camera_config_t;

typedef struct {
    light_config_t light;
    sprinklers_config_t sprinklers;
    camera_config_t camera;
    char *SSID;
    char *password;
    char *server;
} sys_config_t;

extern sys_config_t sys_config;

#endif