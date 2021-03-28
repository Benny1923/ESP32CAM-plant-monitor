#include "include/ws_connect.h"
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

TimerHandle_t ping_timer;

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
        break;
    case WEBSOCKET_EVENT_DATA:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
        if (data->data_len > 2) {
            ESP_LOGI(TAG, "receive data: %s", (char*)data->data_ptr);
        } else {
            esp_websocket_client_send_text(client, "pong", 5, portMAX_DELAY);
        }
        break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
        break;
    }
}

void websocket_app_start(void) {
    esp_websocket_client_config_t websocket_cfg = {};
    websocket_cfg.uri = CONFIG_WEBSOCKET_URI;
    websocket_cfg.ping_interval_sec = 3;    
    ESP_LOGI(TAG, "Connecting to %s...", websocket_cfg.uri);
    client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);
    esp_websocket_client_start(client);
    vTaskDelete(NULL);
}