#include "include/common.h"

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

//sensor collect data interval
#define SENSOR_INTERVAL 5
int count = 0;

sensors_data_t latest_data;

void task_camera(void) {

}

void task_sensor(void) {

}

void task_switch(void) {

}

static char *TAG = "main_task";

//all magic happen in here
void main_task(TimerHandle_t xTimer) {

    //sensor & switch task
    if (count%SENSOR_INTERVAL == 0) {
        ESP_LOGI(TAG, "sensor task triggered at %s", getHourandMinute());
    }

    //camera task
    if (count%sys_config.camera.interval == 0) {
        ESP_LOGI(TAG, "camera task triggered at %s", getHourandMinute());
    }


    count++;
}