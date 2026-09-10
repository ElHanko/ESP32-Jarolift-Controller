#include <LittleFS.h>
#include <Update.h>
#include <basics.h>
#include <language.h>
#include <message.h>
#include <webUI.h>
#include <webUIupdates.h>

const int MAX_WS_CLIENT = 3;
const int CHUNK_SIZE = 1024;

/* P R O T O T Y P E S ********************************************************/
void webCallback(const char *elementId, const char *value);

/* D E C L A R A T I O N S ****************************************************/
static muTimer heartbeatTimer = muTimer(); // timer to refresh other values
static muTimer onLoadTimer = muTimer();    // timer to refresh other values

EspWebUI webUI(80);

static const char *TAG = "WEB"; // LOG TAG
static bool webInitDone = false;
static const size_t BUFFER_SIZE = 512;
static bool onLoadRequest = false;

struct WebCallbackEvent {
  char elementID[32];
  char value[256];
};

static constexpr size_t WEB_CALLBACK_QUEUE_SIZE = 8;
static WebCallbackEvent webCallbackQueue[WEB_CALLBACK_QUEUE_SIZE];
static size_t webCallbackQueueHead = 0;
static size_t webCallbackQueueTail = 0;
static size_t webCallbackQueueCount = 0;
static portMUX_TYPE webCallbackQueueMux = portMUX_INITIALIZER_UNLOCKED;

static auto &wdt = EspSysUtil::Wdt::getInstance();
static auto &ota = EspSysUtil::OTA::getInstance();

/**
 * *******************************************************************
 * @brief   cyclic call for webUI - creates all webUI elements
 * @param   none
 * @return  none
 * *******************************************************************/
void webUISetup() {

  webUI.setCallbackOta([](EspWebUI::otaStatus otaState, const char *msg) {
    switch (otaState) {
    case EspWebUI::OTA_BEGIN:
      ota.setActive(true);
      wdt.disable();
      break;
    case EspWebUI::OTA_PROGRESS:
      webUI.wsUpdateOTAprogress(msg);
      break;
    case EspWebUI::OTA_FINISH:
      ota.setActive(false);
      wdt.enable();
      webUI.wsUpdateOTAprogress("100");
      webUI.wsUpdateWebDialog("ota_update_done_dialog", "open");
      break;
    case EspWebUI::OTA_ERROR:
      ota.setActive(false);
      wdt.enable();
      webUI.wsUpdateWebText("p00_ota_upd_err", msg, false);
      webUI.wsUpdateWebDialog("ota_update_failed_dialog", "open");
      break;
    }
  });

  webUI.setCallbackUpload([](EspWebUI::uploadStatus uploadState, const char *msg) {
    switch (uploadState) {
    case EspWebUI::UPLOAD_BEGIN:
      webUI.wsUpdateWebText("upload_status_txt", msg, false);
      break;
    case EspWebUI::UPLOAD_FINISH:
      webUI.wsUpdateWebText("upload_status_txt", msg, false);
      configLoadFromFile(); // load configuration
      webUI.wsUpdateWebLanguage(LANG::CODE[config.lang]);
      webUI.wsLoadConfigWebUI(); // update webUI settings
      break;
    case EspWebUI::UPLOAD_ERROR:
      webUI.wsUpdateWebText("upload_status_txt", msg, false);
      break;
    }
  });

  // callback for reload
  webUI.setCallbackReload([]() { onLoadRequest = true; });

  // callback for web elements - queue elementID and value for processing in cyclic loop
  webUI.setCallbackWebElement([](const char *elementID, const char *elementValue) {
    WebCallbackEvent event{};
    snprintf(event.elementID, sizeof(event.elementID), "%s", elementID);
    snprintf(event.value, sizeof(event.value), "%s", elementValue);

    bool queued = false;
    portENTER_CRITICAL(&webCallbackQueueMux);
    if (webCallbackQueueCount < WEB_CALLBACK_QUEUE_SIZE) {
      webCallbackQueue[webCallbackQueueTail] = event;
      webCallbackQueueTail = (webCallbackQueueTail + 1) % WEB_CALLBACK_QUEUE_SIZE;
      webCallbackQueueCount++;
      queued = true;
    }
    portEXIT_CRITICAL(&webCallbackQueueMux);

    if (!queued) {
      ESP_LOGW(TAG, "Web callback queue full, dropping event: %s", event.elementID);
    }
  });

  webUI.setCredentials(config.auth.user, config.auth.password);
  webUI.setAuthentication(!setupMode);

  webUI.begin();
} // END SETUP

/**
 * *******************************************************************
 * @brief   cyclic call for webUI - refresh elements by change
 * @param   none
 * @return  none
 * *******************************************************************/
void webUICyclic() {

  webUI.loop();

  // request for update alle elements - not faster than every 1s
  if (onLoadRequest && onLoadTimer.cycleTrigger(1000)) {
    updateAllElements();
    onLoadRequest = false;
    ESP_LOGD(TAG, "updateAllElements()");
  }

  // handling of update webUI elements
  webUIupdates();

  // handling of callback information
  for (size_t processed = 0; processed < WEB_CALLBACK_QUEUE_SIZE; processed++) {
    WebCallbackEvent event{};
    bool eventAvailable = false;

    portENTER_CRITICAL(&webCallbackQueueMux);
    if (webCallbackQueueCount > 0) {
      event = webCallbackQueue[webCallbackQueueHead];
      webCallbackQueueHead = (webCallbackQueueHead + 1) % WEB_CALLBACK_QUEUE_SIZE;
      webCallbackQueueCount--;
      eventAvailable = true;
    }
    portEXIT_CRITICAL(&webCallbackQueueMux);

    if (!eventAvailable) {
      break;
    }

    webCallback(event.elementID, event.value);
  }

  webInitDone = true; // init done
}
