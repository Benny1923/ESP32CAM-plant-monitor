#include "include/sdcard.h"
#include "include/common.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include <sys/time.h>
#include <string.h>

sdmmc_card_t *card;

static char *TAG = "sdcard";

void init_sdcard() {
  esp_err_t ret = ESP_FAIL;
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 3,
  };
  //sdmmc_card_t *card;

  ESP_LOGI(TAG, "Mounting SD card...");
  ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

  if (ret == ESP_OK) {
    ESP_LOGI(TAG, "SD card mount successfully!");
  } else {
    ESP_LOGE(TAG, "Failed to mount SD card VFAT filesystem. Error: %s", esp_err_to_name(ret));
  }
}

void save_file(uint8_t *buf, size_t len, char *filename) {
  FILE *file = fopen(filename, "w");
  if (file == NULL)
    ESP_LOGE(TAG, "Could not open file =( %s", filename);
  else {
    size_t err = fwrite(buf, 1, len, file);
    ESP_LOGI(TAG, "File saved: %s", filename);
  }
  fclose(file);
}

//example timestamp
const char *time_format = "[2020/02/25 13:30:15]";

char *get_time() {
  char *timestamp = (char*) malloc(strlen(time_format)+1);
  time_t now;
  time(&now);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  if (timeinfo.tm_year < (2020-1900)) {
    free(timestamp);
    return "[powerup]";
  } else {
    strftime(timestamp, strlen(time_format)+1, "[%Y/%m/%d %H:%M:%S]", &timeinfo);
    return timestamp;
  }
}

//log feature, save log to log.txt
void save_log(char *unit, char *msg) {
  FILE *file = fopen("/sdcard/log.txt", "a");
  fputs(get_time(),file);
  fputs(" ", file);
  fputs(unit, file);
  fputs(": ", file);
  fputs(msg, file);
  fputs("\n",file);
  fclose(file);
}