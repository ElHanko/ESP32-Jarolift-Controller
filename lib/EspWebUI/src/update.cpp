#include "EspWebUI.h"

namespace {

AsyncWebServerRequest *activeOtaRequest = nullptr;

constexpr char OTA_FAILED_ATTR[] = "ota_failed";

unsigned long lastUpdateTime = 0;
int lastProgress = -1;
size_t firmwareSize = 0;

} // namespace

/**
 * *******************************************************************
 * @brief   function to process the firmware update
 * @param   request, filename, index, data, len, final
 * @return  none
 * *******************************************************************/
void EspWebUI::handleDoUpdate(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {

  // Ignore all remaining chunks of an already failed request.
  if (request->getAttribute(OTA_FAILED_ATTR, false)) {
    return;
  }

  if (!index) {

    // Only one OTA upload may own the global Update instance.
    if (activeOtaRequest != nullptr && activeOtaRequest != request) {
      request->setAttribute(OTA_FAILED_ATTR, true);

      ESP_LOGW(TAG, "webOTA rejected: another upload is already active");

      if (!request->isSent()) {
        request->send(409, "text/plain", "another OTA update is already active");
      }

      return;
    }

    activeOtaRequest = request;
    const long requestedFirmwareSize =
        request->header("X-Firmware-Size").toInt();

    if (requestedFirmwareSize <= 0) {
      request->setAttribute(OTA_FAILED_ATTR, true);
      activeOtaRequest = nullptr;

      ESP_LOGE(TAG, "webOTA rejected: invalid firmware size");
      callbackOta(OTA_ERROR, "Invalid firmware size");

      if (!request->isSent()) {
        request->send(400, "text/plain", "invalid firmware size");
      }

      return;
    }

    firmwareSize = static_cast<size_t>(requestedFirmwareSize);
    lastUpdateTime = 0;
    lastProgress = -1;

    /*
     * If the client disconnects before the upload is complete,
     * abort the Update session and release the OTA lock.
     */
    request->onDisconnect([this, request]() {
      if (activeOtaRequest == request) {
        ESP_LOGW("WEBUI", "webOTA client disconnected");

        if (Update.isRunning()) {
          Update.abort();
        }

        activeOtaRequest = nullptr;
        callbackOta(OTA_ERROR, "OTA Update aborted: client disconnected");
      }
    });

    ESP_LOGI(TAG, "webOTA started: %s", filename.c_str());
    callbackOta(OTA_BEGIN, "OTA Update started");

    if (!Update.begin(firmwareSize, U_FLASH)) {
      String error = Update.errorString();

      if (Update.isRunning()) {
        Update.abort();
      }

      request->setAttribute(OTA_FAILED_ATTR, true);
      activeOtaRequest = nullptr;

      ESP_LOGE(TAG, "webOTA begin failed: %s", error.c_str());
      callbackOta(OTA_ERROR, error.c_str());

      if (!request->isSent()) {
        request->send(400, "text/plain", "OTA could not begin");
      }

      return;
    }

    callbackOta(OTA_PROGRESS, "0");
    lastProgress = 0;

  } else if (activeOtaRequest != request) {

    /*
     * A chunk without ownership must never be written into
     * another request's Update session.
     */
    request->setAttribute(OTA_FAILED_ATTR, true);

    ESP_LOGW(TAG, "webOTA rejected: request does not own OTA session");

    if (!request->isSent()) {
      request->send(409, "text/plain", "OTA session is not active for this request");
    }

    return;
  }

  // Update in progress.
  if (Update.write(data, len) != len) {
    String error = Update.errorString();

    if (Update.isRunning()) {
      Update.abort();
    }

    request->setAttribute(OTA_FAILED_ATTR, true);
    activeOtaRequest = nullptr;

    ESP_LOGE(TAG, "webOTA write failed: %s", error.c_str());
    callbackOta(OTA_ERROR, error.c_str());

    if (!request->isSent()) {
      request->send(400, "text/plain", "OTA write failed");
    }

    return;
  }

  // Calculate and send progress.
  if (firmwareSize > 0) {
    int progress = static_cast<int>(((index + len) * 100ULL) / firmwareSize);
    if (progress > 100) {
      progress = 100;
    }
    unsigned long currentTime = millis();

    if (currentTime - lastUpdateTime >= 1000) {
      lastUpdateTime = currentTime;

      if (progress > lastProgress) {
        callbackOta(OTA_PROGRESS, String(progress).c_str());
        lastProgress = progress;
      }
    }
  }

  // Update done.
  if (final) {

    if (!Update.end()) {
      String error = Update.errorString();

      if (Update.isRunning()) {
        Update.abort();
      }

      request->setAttribute(OTA_FAILED_ATTR, true);
      activeOtaRequest = nullptr;

      ESP_LOGE(TAG, "OTA Update failed: %s", error.c_str());
      callbackOta(OTA_ERROR, error.c_str());

      if (!request->isSent()) {
        request->send(400, "text/plain", "Could not end OTA");
      }

      return;
    }

    activeOtaRequest = nullptr;

    ESP_LOGI(TAG, "OTA Update complete");
    callbackOta(OTA_FINISH, "OTA Update complete");

    if (!request->isSent()) {
      request->send(200, "text/plain", "OTA Update finished!");
    }
  }
}
