#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

//ntp_adj.c
//adjust time with ntp, internet access require
void ntp_adj(void);

//sdcard.c
//save log to /sdcard/log.txt
void save_log(char *unit, char *msg);
void checkdir(char *path);

//cfg_parser.c
//load config from /sdcard/config.txt
esp_err_t load_config(void);
esp_err_t update_config();

//json_parser.c
//create json response, reference api document
char *create_response_msg(int type, void *ptr);

//str.c
char *newstr(size_t len);
void strpad(char **des, char *src);

//time.c
char *getMinute();
int intGetMinute();
char *getHourandMinute();
void addMinute(char *target, int min);
int isWorkTime(char *start, char *end);
char *getYearMonthDay();

//json_parser.c
char *create_response_msg(int type, void *ptr);
void json_parser(char *str);

//task.c
void main_task(TimerHandle_t xTimer);
void dev_ctl(int id, int stat);

//ws_connect.c
void send_data(char *data, size_t len);

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
    int light_sw;
    int sprinklers_sw;
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

extern sensors_data_t latest_data;

#endif
