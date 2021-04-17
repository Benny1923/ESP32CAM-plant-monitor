#include "include/common.h"
#include "include/http_client.h"
#include "include/sdcard.h"
#include "include/i2c_con.h"
#include "include/camera.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>

//sensor collect data interval
#define SENSOR_INTERVAL 5
int count = 0;

sensors_data_t latest_data;

void task_camera(void) {
    if (!sys_config.camera.nightmode) {
        if (!isWorkTime(sys_config.camera.start, sys_config.camera.end)) {
            return;
        }
    }
    char *path = newstr(1);
    strpad(&path, "/sdcard/");
    char *ymd = getYearMonthDay();
    strpad(&path, ymd);
    free(ymd);
    checkdir(path);
    http_post_img_t img = {
        .name = "img"
    };
    img.len = take_picture(&img.buf);
    img.filename = getHourandMinute();
    img.filename[2] = '-';
    strpad(&img.filename, ".jpg");
    strpad(&path, "/");
    strpad(&path, img.filename);
    save_file(img.buf, img.len, path);
    xTaskCreate(&post_img, "post_img", 8192, &img, 5, NULL);
    free(path);
}

void task_sensor(void) {
    adc_read(0, &latest_data.moisture);
    adc_read(1, &latest_data.tank_fluid);
    char *status = create_response_msg(3, &latest_data);
    send_data(status, strlen(status)+1);
    free(status);
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
    if (isWorkTime(sys_config.light.start, sys_config.light.end)) {
        //do something
    }

    if (isWorkTime(sys_config.sprinklers.start, sys_config.sprinklers.end)) {
        //do something
    }

}

static char *TAG = "main_task";

//all magic happen in here
void main_task(TimerHandle_t xTimer) {

    //sensor & switch task
    if (count%SENSOR_INTERVAL == 0) {
        ESP_LOGI(TAG, "sensor task triggered at %s", getHourandMinute());
        task_sensor();
        task_switch();
    }

    //camera task
    if (count%sys_config.camera.interval == 0) {
        ESP_LOGI(TAG, "camera task triggered at %s", getHourandMinute());
        task_camera();
    }


    count++;
}