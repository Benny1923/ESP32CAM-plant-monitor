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
        break;
    default: //wrong type
        return "";
        break;
    }
    string = cJSON_Print(data);
    //printf("%s\n", string);
    return string;
}

void json_parser(char *str) {
}