#include <EspSysUtil.h>

static const char *TAG = "ESU-RST"; // LOG TAG
static char localRestartReason[64];

/**
 * *******************************************************************
 * @brief   get last restart reason as string
 * @param   none
 * @return  none
 * *******************************************************************/
const char *EspSysUtil::RestartReason::get() {

  if (strlen(localRestartReason) != 0) {
    return localRestartReason;
  } else {

    esp_reset_reason_t reason = esp_reset_reason();
    switch (reason) {
    case ESP_RST_UNKNOWN:
      return "Unknown";
    case ESP_RST_POWERON:
      return "Power-on reset";
    case ESP_RST_EXT:
      return "External reset";
    case ESP_RST_SW:
      return "Software reset via esp_restart";
    case ESP_RST_PANIC:
      return "Software panic reset";
    case ESP_RST_INT_WDT:
      return "Interrupt Watchdog";
    case ESP_RST_TASK_WDT:
      return "Task Watchdog";
    case ESP_RST_WDT:
      return "Other Watchdog";
    case ESP_RST_DEEPSLEEP:
      return "Deep Sleep Exit";
    case ESP_RST_BROWNOUT:
      return "Brownout (voltage dip)";
    case ESP_RST_SDIO:
      return "Reset over SDIO";
#if ESP_IDF_VERSION_MAJOR >= 5
    case ESP_RST_USB:
      return "USB Peripheral";
    case ESP_RST_JTAG:
      return "JTAG";
    case ESP_RST_EFUSE:
      return "EFUSE Error";
    case ESP_RST_PWR_GLITCH:
      return "Power Glitch";
    case ESP_RST_CPU_LOCKUP:
      return "CPU Lockup";
#endif
    default:
      return "Unknown";
    }
  }
}

/**
 * *******************************************************************
 * @brief   save restart reason to file
 * @param   reason
 * @return  none
 * *******************************************************************/
void EspSysUtil::RestartReason::saveLocal(const char *reason) {

  File file = LittleFS.open("/restart_reason.txt", "w");
  if (!file) {
    ESP_LOGE(TAG, "Failed to open file for writing");
    return;
  }
  file.println(reason);
  file.close();
}

/**
 * *******************************************************************
 * @brief   read restart reason from file
 * @param   buffer
 * @param   bufferSize
 * @return  none
 * *******************************************************************/
bool EspSysUtil::RestartReason::readLocal() {
  if (!LittleFS.exists("/restart_reason.txt")) {
    return false; // no file
  }

  File file = LittleFS.open("/restart_reason.txt", "r");
  if (!file) {
    ESP_LOGE(TAG, "Failed to open file for reading");
    return false;
  }

  bool result = false;
  if (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    strncpy(localRestartReason, line.c_str(), sizeof(localRestartReason) - 1);
    localRestartReason[sizeof(localRestartReason) - 1] = '\0';
    result = true;
  }
  file.close();

  LittleFS.remove("/restart_reason.txt");

  return result;
}