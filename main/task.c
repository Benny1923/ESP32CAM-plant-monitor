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
int light_status = 0;

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
    static http_post_img_t img = {
        .name = "img"
    };
    if (!light_status) {
        dev_ctl(1, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    img.len = take_picture(&img.buf);
    if (!light_status) dev_ctl(1, 0);
    img.filename = getHourandMinute();
    img.filename[2] = '-';
    strpad(&img.filename, ".jpg");
    strpad(&path, "/");
    strpad(&path, img.filename);
    save_file(img.buf, img.len, path);
    xTaskCreate(&post_img, "post_img", 8192, &img, 5, NULL);
    free(path);
}


#define RAW_lux_MAX 29760
#define RAW_lux_MIN 80
#define RAW_moisture_MAX 100
#define RAW_moisture_MIN 0
#define RAW_tank_fluid_MAX 100 
#define RAW_tank_fluid_MIN 0
//raw data converter
//type: 1.lux 2.moisture 3.tank_fulid
void raw_data_converter(int type, uint16_t *raw) {
    int unit = 1;
    int low = 0;
    switch (type) {
    case 1:
        unit = (RAW_lux_MAX - RAW_lux_MIN) / 100;
        low = RAW_lux_MIN;
        break;
    case 2:
        unit = (RAW_moisture_MAX - RAW_moisture_MIN) / 100;
        low = RAW_moisture_MIN;
        break;
    case 3:
        unit = (RAW_tank_fluid_MAX - RAW_tank_fluid_MIN) / 100;
        low = RAW_tank_fluid_MIN;
        break;
    default:
        break;
    }
    *raw = (*raw - low) / unit;
}

//sensor task: collect data and send to server
void task_sensor(void) {
    adc_read(0, &latest_data.moisture);
    raw_data_converter(2, &latest_data.moisture);
    adc_read(1, &latest_data.tank_fluid);
    raw_data_converter(3, &latest_data.tank_fluid);
    char *status = create_response_msg(3, &latest_data);
    send_data(status, strlen(status));
    free(status);
    char *log = newstr(strlen("adc0: 12345, adc1: 12345")+1);
    sprintf(log, "adc0: %.5d, adc1: %.5d", latest_data.moisture, latest_data.tank_fluid);
    save_log("task_sensor", log);
    free(log);
}

//relay control
//id: 1: gpio0 2: gpio1 3: all
//stat 1: on 0:off
void dev_ctl(int id, int stat) {
    switch (id) {
    case 1:
        pcf8574t_gpio_set(0, !stat);
        latest_data.light_sw = stat;
        break;
    case 2:
        pcf8574t_gpio_set(1, !stat);
        latest_data.sprinklers_sw = stat;
        break;
    case 3:
        pcf8574t_gpio_set(0, !stat);
        pcf8574t_gpio_set(1, !stat);
        latest_data.light_sw = stat;
        latest_data.sprinklers_sw = stat;
        break;
    default:
        //do nothing
        break;
    }
}


#define sprinklers_turnon_SEC 10
void task_switch(void) {
    if (isWorkTime(sys_config.light.start, sys_config.light.end)) {
        //do something
        if (latest_data.lux >= sys_config.light.max) {
            dev_ctl(2, 0);
            light_status = 0;
        } else if (latest_data.lux <= sys_config.light.min) {
            dev_ctl(2, 1);
            light_status = 1;
        }
    }

    if (isWorkTime(sys_config.sprinklers.start, sys_config.sprinklers.end)) {
        //do something
        if (latest_data.moisture <= sys_config.sprinklers.min) {
            dev_ctl(1, 1);
            vTaskDelay(sprinklers_turnon_SEC * 1000 / portTICK_PERIOD_MS);
            dev_ctl(1, 0);
        }
    }

}

static char *TAG = "main_task";

//all magic happen in here
void main_task(TimerHandle_t xTimer) {

    //sensor & switch task
    if (count%SENSOR_INTERVAL == 0) {
        ESP_LOGI(TAG, "sensor task triggered at %s", getHourandMinute());
        save_log(TAG, "sensor task triggered");
        task_sensor();
        task_switch();
    }

    //camera task
    if (count%sys_config.camera.interval == 0) {
        ESP_LOGI(TAG, "camera task triggered at %s", getHourandMinute());
        save_log(TAG, "camera task triggered");
        task_camera();
    }

    count++;

    //do nothing require by free rtos
    count = count;
}