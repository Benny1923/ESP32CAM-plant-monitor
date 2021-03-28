#ifndef WIFI_CONN_H
#define WIFI_CONN_H
#include "esp_err.h"

#define ESP_CAM_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define ESP_CAM_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define ESP_CAM_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

esp_err_t wifi_init_sta(char *SSID, char *password);

#endif