#include "include/ws_connect.h"
#include "include/common.h"
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "esp_websocket_client.h"

#define WS_RETRY_COUNT_MAX 10
#define CONFIG_WEBSOCKET_URI "ws://" CONFIG_SERVER_ADDR ":8080/websocket"

static char *TAG = "websocket";

static esp_websocket_client_handle_t client;

TimerHandle_t ping_pong_timer;

static int ping_pong_count = 0;

static void ping_pong_counter(TimerHandle_t xTimer) {
    ping_pong_count += 1;
}

void send_data(char *data, size_t len) {
    if (esp_websocket_client_is_connected(client)) {
        esp_websocket_client_send_text(client, data, len, portMAX_DELAY);
        ping_pong_count = 0;
    }
    return;
}

char *msg_buffer;

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
        xTimerStart(ping_pong_timer, portMAX_DELAY);
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
        xTimerStop(ping_pong_timer, 0);
        break;
    case WEBSOCKET_EVENT_DATA:
        //ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
        if (data->data_len > 2) {
            ESP_LOGI(TAG, "receive data: %.*s", data->data_len, (char*)data->data_ptr);
            msg_buffer = newstr(data->data_len+1);
            strncpy(msg_buffer, data->data_ptr, data->data_len);
            json_parser(msg_buffer);
        } else {
            if (ping_pong_count >= 3) {
                esp_websocket_client_send_text(client, "pong", 5, portMAX_DELAY);
                ping_pong_count = 0;
            }
        }
        break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
        xTimerStop(ping_pong_timer, 0);
        break;
    }
}

void websocket_app_start(void) {
    esp_websocket_client_config_t websocket_cfg = {};
    if (sys_config.server != NULL) {
        char *url = newstr(1);
        strpad(&url, "ws://");
        strpad(&url, sys_config.server);
        strpad(&url, ":8080/websocket");
        websocket_cfg.uri = url;
    } else {
        websocket_cfg.uri = CONFIG_WEBSOCKET_URI;
    }
    websocket_cfg.ping_interval_sec = 3;
    ping_pong_timer = xTimerCreate("ping pong timer", 1000 / portTICK_PERIOD_MS, pdTRUE, NULL, ping_pong_counter);
    ESP_LOGI(TAG, "Connecting to %s...", websocket_cfg.uri);
    client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);
    esp_websocket_client_start(client);
    vTaskDelete(NULL);
}
