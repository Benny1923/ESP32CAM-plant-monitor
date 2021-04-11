#include "include/common.h"
#include "include/http_client.h"
#include "include/sdcard.h"
#include "include/i2c_con.h"
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

//relay control
//id: 1: gpio0 2: gpio1 3: all
//stat 1: on 0:off
void dev_ctl(int id, int stat) {
    switch (id) {
    case 1:
        pcf8574t_gpio_set(1, stat);
        latest_data.light_sw = stat;
        break;
    case 2:
        pcf8574t_gpio_set(2, stat);
        latest_data.sprinklers_sw = stat;
        break;
    case 3:
        pcf8574t_gpio_set(1, stat);
        pcf8574t_gpio_set(2, stat);
        latest_data.light_sw = stat;
        latest_data.sprinklers_sw = stat;
        break;
    default:
        //do nothing
        break;
    }
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