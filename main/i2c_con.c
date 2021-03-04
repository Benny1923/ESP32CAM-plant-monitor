#include "include/i2c_con.h"
#include "include/BH1750.h"
#include "include/ADS1115.h"
#include "include/PCF8574T.h"
#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"

static char *TAG = "i2c";

static esp_err_t i2c_master_init(void) {
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) {
        return err;
    }
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

//初始化I2C
esp_err_t wire_begin() {
    int ret = i2c_master_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "cannot setup I2C");
    }
    return ret;
}

//亮度感測器設定
esp_err_t lux_setup() {
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, BH1750_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, CONTINUOUS_HIGH_RES_MODE, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "cannot setup BH1750");
        return ret;
    }
    return ret;
}

//亮度感測器
esp_err_t lux_read(uint8_t *data) {
    uint8_t data_h = 0, data_l = 0;
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, BH1750_SENSOR_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &data_h, ACK_VAL);
    i2c_master_read_byte(cmd, &data_l, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    *data = (data_h << 8 | data_l);
    ESP_LOGI(TAG, "BH1750: %.02flux", (data_h << 8 | data_l) / 1.2);
    return ret;
}

//AD轉換(單一頻道)
esp_err_t adc_read(uint8_t ch, uint8_t *data) {
    uint8_t data_h = 0, data_l = 0;
    // Start with default values
    uint16_t config =
        ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
        ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
        ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
        ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
        ADS1015_REG_CONFIG_DR_1600SPS |   // 1600 samples per second (default)
        ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    // Set PGA/voltage range
    config |= ADS1015_REG_CONFIG_PGA_6_144V;

    // Set single-ended input channel
    switch (ch) {
        case (0):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
            break;
        case (1):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
            break;
        case (2):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
            break;
        case (3):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
            break;
    }

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;

    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ADS1015_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, (uint8_t)ADS1015_REG_POINTER_CONFIG, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, (uint8_t)(config >> 8), ACK_CHECK_EN);
    i2c_master_write_byte(cmd, (uint8_t)(config & 0xFF), ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "cannot setup ADC(%d)", ch);
        return ret;
    }
    vTaskDelay(9 / portTICK_PERIOD_MS);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ADS1015_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, ADS1015_REG_POINTER_CONVERT, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "cannot setup ADC(%d) read register", ch);
        return ret;
    }
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ADS1015_ADDRESS << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &data_h, ACK_VAL);
    i2c_master_read_byte(cmd, &data_l, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "cannot read ADC(%d)", ch);
        return ret;
    }
    *data = (data_h << 8 | data_l);
    ESP_LOGI(TAG, "ADS1115(%d): %d", ch, (data_h << 8 | data_l));
    return ret;
}

static uint8_t gpio_status = 0x00;

//PCF8574T GPIO設定
esp_err_t pcf8574t_gpio_set(int pin, int enable) {
    uint8_t config = 0;
    switch (pin) {
    case (0):
        config |= PCF8574T_GPIO_0;
        break;
    case (1):
        config |= PCF8574T_GPIO_1;
        break;
    case (2):
        config |= PCF8574T_GPIO_2;
        break;
    case (3):
        config |= PCF8574T_GPIO_3;
        break;
    case (4):
        config |= PCF8574T_GPIO_4;
        break;
    case (5):
        config |= PCF8574T_GPIO_5;
        break;
    case (6):
        config |= PCF8574T_GPIO_6;
        break;
    case (7):
        config |= PCF8574T_GPIO_7;
        break;
    default:
        ESP_LOGI(TAG, "PCF8574T: channel doesn't exist, do nothing.");
        return ESP_OK;
        break;
    }

    if (enable) {
        gpio_status |= config;
    } else {
        gpio_status &= ~config;
    }

    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, PCF8574T_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, gpio_status, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "cannot setup PCF8574T(%d)", pin);
        return ret;
    }
    return ret;
}