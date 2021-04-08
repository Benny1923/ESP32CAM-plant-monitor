#include "include/common.h"
#include <stdio.h>
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
        cJSON_AddStringToObject(status, "lux", ((sensors_data_t*)ptr)->lux);
        cJSON_AddStringToObject(status, "moisture", ((sensors_data_t*)ptr)->moisture);
        cJSON_AddStringToObject(status, "tankfluid", ((sensors_data_t*)ptr)->tank_fluid);
        cJSON_AddIntToObject(status, "light_sw", ((sensors_data_t*)ptr)->light_sw);
        cJSON_AddIntToObject(status, "sprinklers_sw", ((sensors_data_t*)ptr)->sprinklers_sw);
        break;
    default: //wrong type
        return "";
        break;
    }
    string = cJSON_Print(data);
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
    } else if (strcmp(op->valuestring, "update") == 0) {
        //update handler
    } else if (strcmp(op->valuestring, "reboot") == 0) {
        //reboot handler
    }
}