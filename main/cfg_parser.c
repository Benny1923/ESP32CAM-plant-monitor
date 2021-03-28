#include "include/common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "esp_system.h"
#include "esp_log.h"

#define CFG_PATH "/sdcard/config.txt"

static char *TAG = "cfg_parser";

struct dataset{
    char *tag;
    void **value;
    char *type;
};

//全域系統參數
sys_config_t sys_config;

//解析器綁定
struct dataset data[] = {
    {"SSID", &sys_config.SSID, "char"},
    {"password", &sys_config.password, "char"},
    {"server", &sys_config.server, "char"},
    {"light-start", &sys_config.light.start, "char"},
    {"light-end", &sys_config.light.end, "char"},
    {"light-max", &sys_config.light.min, "int"},
    {"light-min", &sys_config.light.max, "int"},
    {"sprinklers-start", &sys_config.sprinklers.start, "char"},
    {"sprinklers-end", &sys_config.sprinklers.end, "char"},
    {"sprinklers-min", &sys_config.sprinklers.min, "int"},
    {"camera-start", &sys_config.camera.start, "char"},
    {"camera-end", &sys_config.camera.end, "char"},
    {"camera-interval", &sys_config.camera.interval, "int"},
    {"camera-nightmode", &sys_config.camera.nightmode, "int"},
};

//load config to sys_config
esp_err_t load_config(void) {
    if (access(CFG_PATH, R_OK | W_OK) == -1) {
        ESP_LOGE(TAG, "cannot access %s", CFG_PATH);
        goto fail;
    }
    char *line = malloc(60);
    int len = 60;
    char *pos;
    FILE *file = fopen(CFG_PATH, "r");
    while(fgets(line, len, file)!=NULL) {
        if (line[0] == '#') continue;
        for(int i=0 ; i<(sizeof(data)/sizeof(struct dataset)) ; i++) {
            if (strstr(line, data[i].tag)!=NULL) {
                if (strcmp(data[i].type, "char") == 0) {
                    if ((pos = strchr(line, '\n')) != NULL) *pos = '\0';
                    if ((pos = strchr(line, '\r')) != NULL) *pos = '\0';
                    *data[i].value = malloc(strlen(strchr(line, '='))+1);
                    strcpy(*data[i].value, strchr(line, '=') + sizeof(char));
                    break;
                } else if (strcmp(data[i].type, "int") == 0) {
                    *data[i].value = atoi(strchr(line, '=') + sizeof(char));
                    break;
                }
            }
        }
    }
    fclose(file);

    for(int i=0 ; i<(sizeof(data)/sizeof(struct dataset)); i++) {
        if (strcmp(data[i].type, "char") == 0) {
            ESP_LOGI(TAG, "%s: %s", data[i].tag, (char *)*data[i].value);
        } else if (strcmp(data[i].type, "int") == 0) {
            ESP_LOGI(TAG, "%s: %d", data[i].tag, (int)*data[i].value);
        }
    }

    return ESP_OK;
    fail: 
    return ESP_FAIL;
}

//update config.txt from sys_config
//unsupport wifi SSID and password
//not done yet
esp_err_t update_config() {
    if (access(CFG_PATH, R_OK | W_OK) == -1) {
        ESP_LOGE(TAG, "cannot access %s", CFG_PATH);
        goto fail;
    }
    FILE *file = fopen(CFG_PATH, "r+");
    char *line = malloc(60);
    int len = 60;
    while(fgets(line, len, file)!=NULL) {
        
    }
    fclose(file);
    return ESP_OK;
    fail:
    return ESP_FAIL;
}