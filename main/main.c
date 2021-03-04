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

#include "include/common.h"
#include "include/wifi_connection.h"
#include "include/sdcard.h"
#include "include/camera.h"
#include "include/i2c_con.h"
#include "include/http_client.h"
#include "include/ws_connect.h"

static char *TAG = "setup";

#include "freertos/event_groups.h"


//hardware self test, but not include internet connection
esp_err_t self_test(void) {
    char *unit = "self_test";
    ESP_LOGI(unit, "staring self test");
    esp_err_t ret = ESP_OK;
    uint8_t data = 0;
    //ret = lux_read(&data);
    // if (data > 300) ret = ESP_FAIL; //data incorrect
    if (ret != ESP_OK) goto fail;
    ret = adc_read(0, &data);
    ret = adc_read(1, &data);
    if (ret != ESP_OK) goto fail;
    pcf8574t_gpio_set(0, 1);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    pcf8574t_gpio_set(1, 1);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    pcf8574t_gpio_set(1, 0);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    pcf8574t_gpio_set(0, 0);
    return ESP_OK;
    //when fail
    fail:
        return ESP_FAIL;
}

void app_main(void) {
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

    esp_err_t ret;
    //mounting sdcard
    init_sdcard();
    //initialize camera
    ret = init_camera();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "fail to initialize camera");
        goto end;
    }
    //system setup
    ret = wire_begin();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "fail to initialize I2C");
        //goto end;
    }
    //ret = lux_setup();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "fail to initialize BH1750: %s", esp_err_to_name(ret));
        goto end;
    }
    ret = self_test();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "self test fail");
        goto end;
    } else {
        ESP_LOGI(TAG, "self test success");
    }

    //when something fail
    end:
        ESP_LOGI(TAG, "system stopped");
    // fflush(stdout);
    // esp_restart();
}
