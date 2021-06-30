#include "include/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

//回應訊息
//1:ping訊息(NULL) 2:訊息(char指標) 3:系統狀態(sensor_data_t指標)
char *create_response_msg(int type, void *ptr) {
    char *string = NULL;
    cJSON *data = cJSON_CreateObject();
    switch (type) {
    case 1:
        cJSON_AddStringToObject(data, "type", "ping");
        break;
    case 2:
        cJSON_AddStringToObject(data, "type", "message");
        cJSON_AddStringToObject(data, "msg", (char*)ptr);
        break;
    case 3:
        cJSON_AddStringToObject(data, "type", "status");
        cJSON *status = cJSON_AddObjectToObject(data, "status");
        cJSON_AddNumberToObject(status, "lux", ((sensors_data_t*)ptr)->lux);
        cJSON_AddNumberToObject(status, "moisture", ((sensors_data_t*)ptr)->moisture);
        cJSON_AddNumberToObject(status, "tankfluid", ((sensors_data_t*)ptr)->tank_fluid);
        cJSON_AddBoolToObject(status, "light_sw", ((sensors_data_t*)ptr)->light_sw);
        cJSON_AddBoolToObject(status, "sprinklers_sw", ((sensors_data_t*)ptr)->sprinklers_sw);
        break;
    default: //wrong type
        return "";
        break;
    }
    string = cJSON_PrintUnformatted(data);
    //printf("%s\n", string);
    return string;
}

void manual_parser(cJSON *command_json);
void update_parser(cJSON *config_json);

//接收指令
//manual:手動操作 update:更新設定 reboot:重新開機
void json_parser(char *str) {
    cJSON *command_json = NULL;
    command_json = cJSON_Parse(str);
    if (command_json == NULL ){
        char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            //error handler
        }
        return;
    }
    cJSON *op;
    op = cJSON_GetObjectItemCaseSensitive(command_json, "op");
    if (strcmp(op->valuestring, "manual") == 0) {
        //manual handler
        manual_parser(cJSON_GetObjectItemCaseSensitive(command_json, "device"));
    } else if (strcmp(op->valuestring, "update") == 0) {
        //update handler
        update_parser(cJSON_GetObjectItemCaseSensitive(command_json, "config"));
    } else if (strcmp(op->valuestring, "reboot") == 0) {
        //reboot handler
        printf("reboot message recived.");
        esp_restart();
    }
    cJSON_Delete(command_json);
}

void manual_parser(cJSON *command_json) {
    int dev_on = 0;
    int dev_off = 0;
    if (cJSON_HasObjectItem(command_json, "light")) {
        if (cJSON_GetObjectItemCaseSensitive(command_json, "light")->valueint) {
            dev_on = dev_on | 0x02;
        } else {
            dev_off = dev_off | 0x02;
        }
    }
    if (cJSON_HasObjectItem(command_json, "sprinklers")) {
        if (cJSON_GetObjectItemCaseSensitive(command_json, "sprinklers")->valueint) {
            dev_on = dev_on | 0x01;
        } else {
            dev_off = dev_off | 0x01;
        }
    }
    if (dev_on) {
        dev_ctl(dev_on, 1);
    }
    if (dev_off) {
        dev_ctl(dev_off, 0);
    }
    char *msg = create_response_msg(3, &latest_data);
    send_data(msg, strlen(msg));
}

//解析器綁定
//與cfg_parser.c一致
//考慮整合中
struct ds{
    char *tag;
    void **value;
    char *type;
};
struct ds dt[] = {
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

void update_parser(cJSON *config_json) {
    cJSON *temp;
    for(int i=0 ; i<(sizeof(dt)/sizeof(struct ds)) ; i++) {
        if (cJSON_HasObjectItem(config_json, dt[i].tag)) {
            temp = cJSON_GetObjectItemCaseSensitive(config_json, dt[i].tag);
            if (strcmp(dt[i].type, "char") == 0) {
                free(*dt[i].value);
                *dt[i].value = malloc(strlen(cJSON_GetStringValue(temp))+1);
                strcpy(*dt[i].value, cJSON_GetStringValue(temp));
            } else if (strcmp(dt[i].type, "int") == 0) {
                *dt[i].value = temp->valueint;
            }
        }
    }
    update_config();
}