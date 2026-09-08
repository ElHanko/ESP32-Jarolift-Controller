// includes
#include <basics.h>
#include <config.h>
#include <jarolift.h>
#include <message.h>
#include <mqtt.h>
#include <webUI.h>
#include <webUIupdates.h>

/* D E C L A R A T I O N S ****************************************************/
static muTimer heartbeat = muTimer();      // timer for heartbeat signal
static muTimer setupModeTimer = muTimer(); // timer for heartbeat signal
static muTimer wdtTimer = muTimer();       // timer to reset wdt

static EspSysUtil::MRD32 *mrd;  // Multi-Reset-Detector
static bool main_reboot = true; // reboot flag

static const char *TAG = "MAIN"; // LOG TAG

static auto &wdt = EspSysUtil::Wdt::getInstance();

/**
 * *******************************************************************
 * @brief   Main Setup routine
 * @param   none
 * @return  none
 * *******************************************************************/
void setup() {

  // Message Service Setup (before use of MY_LOGx)
  messageSetup();

  // check for double reset
  mrd = new EspSysUtil::MRD32(MRD_TIMEOUT, MRD_RETRIES);
  if (mrd->detectMultipleResets()) {
    ESP_LOGI(TAG, "SETUP-MODE-REASON: MRD detected");
    setupMode = true;
  }

  // initial configuration (can also activate the Setup Mode)
  configSetup();

  // setup watchdog timer
  if (!setupMode) {
    wdt.enable();
  }

  // basic setup functions
  basicSetup();

  // jarolift setup
  if (!setupMode) {
    jaroliftSetup();
  }
  // webUI Setup
  webUISetup();

}

/**
 * *******************************************************************
 * @brief   Main Loop
 * @param   none
 * @return  none
 * *******************************************************************/
void loop() {

  // reset watchdog
  if (wdt.isActive() && wdtTimer.cycleTrigger(2000)) {
    esp_task_wdt_reset();
  }

  // double reset detector
  mrd->loop();

  // webUI Cyclic
  webUICyclic();

  // Message Service
  messageCyclic();

  // check if config has changed
  configCyclic();

  // jarolift code
  if (!setupMode) {
    jaroliftCyclic();
  }

  // check WiFi - automatic reconnect
  if (!setupMode) {
    checkWiFi();
  }

  // check MQTT - automatic reconnect
  if (config.mqtt.enable && !setupMode) {
    mqttCyclic();
  }

  if (setupMode) {
    // LED to Signal Setup-Mode
    if (config.gpio.led_setup <= 0) {
      digitalWrite(LED_BUILTIN, setupModeTimer.cycleOnOff(100, 500));
    } else {
      digitalWrite(config.gpio.led_setup, setupModeTimer.cycleOnOff(100, 500));
    }
  }

  main_reboot = false; // reset reboot flag
}
