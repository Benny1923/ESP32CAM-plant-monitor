#include "include/sdcard.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"

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