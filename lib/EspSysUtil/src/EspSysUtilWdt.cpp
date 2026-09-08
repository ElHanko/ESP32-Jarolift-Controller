#include <EspSysUtil.h>

static const char *TAG = "ESU-WDT"; // LOG TAG

// Watchdog functions
EspSysUtil::Wdt::Wdt(uint32_t timeout, uint8_t idle_core_mask, bool trigger_panic) : isInitialized(false), loopTaskHandle(nullptr) {
  twdt_config = {timeout_ms : timeout, idle_core_mask : idle_core_mask, trigger_panic : trigger_panic};
}

EspSysUtil::Wdt &EspSysUtil::Wdt::getInstance(uint32_t timeout, uint8_t idle_core_mask, bool trigger_panic) {
  static Wdt instance(timeout, idle_core_mask, trigger_panic);
  return instance;
}

void EspSysUtil::Wdt::enable() {

  if (isInitialized) {
    ESP_LOGW(TAG, "Watchdog timer is already initialized");
    return;
  }

  if (loopTaskHandle == nullptr) {
    loopTaskHandle = xTaskGetCurrentTaskHandle();
  }

  if (esp_task_wdt_reconfigure(&twdt_config) == ESP_OK) {
    esp_task_wdt_add(loopTaskHandle);
    ESP_LOGI(TAG, "Watchdog timer initialized for Loop Task");
    isInitialized = true;
  } else {
    ESP_LOGE(TAG, "Failed to initialize Watchdog timer");
  }
}

void EspSysUtil::Wdt::disable() {

  if (!isInitialized) {
    ESP_LOGW(TAG, "Watchdog timer is not initialized");
    return;
  }

  if (loopTaskHandle == nullptr) {
    loopTaskHandle = xTaskGetCurrentTaskHandle();
  }

  esp_task_wdt_delete(loopTaskHandle);
  ESP_LOGI(TAG, "Watchdog timer de-initialized");
  isInitialized = false;
}

bool EspSysUtil::Wdt::isActive() const { return isInitialized; }