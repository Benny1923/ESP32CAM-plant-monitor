/* ESP32CAM_PLANT_MONITOR
   this program is made by Benny1923
   part of the potted monitor system
*/
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include <time.h>
#include <sys/time.h>
#include <dirent.h>

#include "include/wifi_connection.h"
#include "include/ntp_adj.h"
#include "include/sdcard.h"
#include "include/camera.h"
#include "include/i2c_con.h"

static char *TAG = "initial program";

void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    // wifi_init_sta();

    // ntp_adj();
    
    // time_t now;
    // struct tm timeinfo;
    // char strftime_buf[64];
    // time(&now);
    // setenv("TZ", "CST-8", 1);
    // tzset();
    // localtime_r(&now, &timeinfo);
    // if (timeinfo.tm_year < (2016 - 1900)) {
    //     wifi_init_sta();
    //     ntp_adj();
    //     time(&now);
    // }
    // localtime_r(&now, &timeinfo);
    // strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    // ESP_LOGI(TAG, "The current date/time in Taipei is: %s", strftime_buf);

    // init_sdcard();

    // DIR *d;
    // struct dirent *dir;
    // d = opendir("/sdcard");
    // if (d) {
    //     while((dir=readdir(d)) != NULL) {
    //         printf("%s\n", dir->d_name);
    //     }
    //     closedir(d);
    // }

    // init_camera();
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // uint8_t *buf;
    // size_t len;
    // len = take_picture(&buf);
    // char *path = malloc(strlen("/sdcard/pic_hh-MM-ss.jpg")+1);
    // char pic_time[9];
    // strftime(pic_time, 9, "%H-%M-%S", &timeinfo);
    // sprintf(path, "/sdcard/pic_%.8s.jpg", pic_time);
    // save_file(buf, len, path);

    wire_begin();
    lux_setup();
    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        lux_read();
        adc_read(0);
        adc_read(1);
    }

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
