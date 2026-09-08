
#include <EspSysUtil.h>

#include <nvs.h>
#include <nvs_flash.h>

#define NVS_NAMESPACE "mrd_ns"

static const char *TAG = "ESU-MRD"; // LOG TAG

EspSysUtil::MRD32::MRD32(uint32_t timeout, uint32_t requiredResets) {
  esp_err_t err = nvs_flash_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "NVS init failed: %d", err);
  }

  this->timeout = timeout * 1000;
  this->requiredResets = requiredResets;
  this->resetCount = 0;
  this->lastResetTime = 0;
  waitingForDoubleReset = false;
}

bool EspSysUtil::MRD32::detectMultipleResets() {
  loadResetStateNVS();

  unsigned long currentTime = millis();
  if (resetCount == 0 || (currentTime - lastResetTime) < timeout) {
    resetCount++;
    lastResetTime = currentTime;
    saveResetStateNVS();
  } else {
    resetCount = 1; // Reset der Zählung, da Timeout überschritten
    lastResetTime = currentTime;
    saveResetStateNVS();
  }

  if (resetCount >= requiredResets) {
    ESP_LOGI(TAG, "Multiple resets detected (%d/%d)", resetCount, requiredResets);
    clearResetStateNVS();
    return true;
  }

  ESP_LOGD(TAG, "Reset count: %d/%d", resetCount, requiredResets);
  waitingForDoubleReset = true;
  return false;
}

bool EspSysUtil::MRD32::waitingForDRD() { return waitingForDoubleReset; }

void EspSysUtil::MRD32::loop() {
  if (waitingForDoubleReset && millis() > timeout) {
    ESP_LOGD(TAG, "Timeout reached, resetting count");
    stop();
  }
}

void EspSysUtil::MRD32::stop() {
  clearResetStateNVS();
  waitingForDoubleReset = false;
}

void EspSysUtil::MRD32::loadResetStateNVS() {
  nvs_handle_t nvs_handle;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "NVS open failed");
    return;
  }

  size_t required_size = sizeof(resetCount);
  err = nvs_get_blob(nvs_handle, "reset_count", &resetCount, &required_size);
  if (err != ESP_OK)
    resetCount = 0;

  required_size = sizeof(lastResetTime);
  err = nvs_get_blob(nvs_handle, "last_reset_time", &lastResetTime, &required_size);
  if (err != ESP_OK)
    lastResetTime = 0;

  nvs_close(nvs_handle);
  ESP_LOGD(TAG, "Loaded reset count: %d, last reset time: %lu", resetCount, lastResetTime);
}

void EspSysUtil::MRD32::saveResetStateNVS() {
  nvs_handle_t nvs_handle;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "NVS open failed");
    return;
  }

  err = nvs_set_blob(nvs_handle, "reset_count", &resetCount, sizeof(resetCount));
  err |= nvs_set_blob(nvs_handle, "last_reset_time", &lastResetTime, sizeof(lastResetTime));
  err |= nvs_commit(nvs_handle);

  if (err == ESP_OK) {
    ESP_LOGD(TAG, "Saved reset state: %d, time: %lu", resetCount, lastResetTime);
  } else {
    ESP_LOGE(TAG, "Failed to save reset state");
  }

  nvs_close(nvs_handle);
}

void EspSysUtil::MRD32::clearResetStateNVS() {
  nvs_handle_t nvs_handle;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "NVS open failed");
    return;
  }

  resetCount = 0;
  lastResetTime = 0;
  nvs_set_blob(nvs_handle, "reset_count", &resetCount, sizeof(resetCount));
  nvs_set_blob(nvs_handle, "last_reset_time", &lastResetTime, sizeof(lastResetTime));
  nvs_commit(nvs_handle);
  nvs_close(nvs_handle);

  ESP_LOGD(TAG, "Cleared reset state");
}
