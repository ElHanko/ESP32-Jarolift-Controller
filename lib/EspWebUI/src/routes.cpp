#include "EspWebUI.h"

#include <favicon.h>

#include <gzip_lib_css.h>
#include <gzip_lib_js.h>
#include <gzip_login_html.h>
#include <gzip_ntp_html.h>

namespace {
AsyncWebServerRequest *activeConfigUploadRequest = nullptr;

constexpr char CONFIG_UPLOAD_TEMP_FILENAME[] = "/config-upload.tmp";
constexpr char CONFIG_UPLOAD_STATUS_ATTRIBUTE[] = "configUploadStatus";
} // namespace

// Generic function to handle gzip-compressed chunked responses with customizable chunk size
void EspWebUI::sendGzipChunkedResponse(AsyncWebServerRequest *request, const uint8_t *content, size_t contentLength, const char *contentType,
                                    bool checkAuth, size_t chunkSize) {

  // check if authenticated
  if (!isAuthenticated(request) && checkAuth) {
    request->redirect("/login");
    return;
  }

  // Set ETag based on the size of the gzip-compressed file
  char etag[20];
  snprintf(etag, sizeof(etag), "%d", contentLength);

  // Check if the client already has the current version in cache
  if (request->header("If-None-Match") == etag) {
    request->send(304); // 304 Not Modified
    ESP_LOGD(TAG, "contend not changed: %s", request->url().c_str());
    return;
  }

  ESP_LOGD(TAG, "sending: %s", request->url().c_str());
  // Create a chunked response with the specified chunk size
  AsyncWebServerResponse *response =
      request->beginChunkedResponse(contentType, [content, contentLength, chunkSize](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
        // Check if we have reached the end of the file
        if (index >= contentLength) {
          return 0; // End transmission
        }
        // Determine the actual chunk size to send, ensuring we don't exceed maxLen or remaining content length
        size_t actualChunkSize = min(chunkSize, min(maxLen, contentLength - index));
        memcpy(buffer, content + index, actualChunkSize);
        return actualChunkSize; // Return the number of bytes sent
      });

  // Set HTTP headers
  response->addHeader(asyncsrv::T_Content_Encoding, "gzip");             // Gzip-Encoding
  response->addHeader(asyncsrv::T_Cache_Control, "public,max-age=60");   // Cache-Control header
  response->addHeader(asyncsrv::T_ETag, etag);                           // Set ETag based on file size
  response->addHeader(asyncsrv::T_Last_Modified, getLastModifiedDate()); // Set Last-Modified to build date

  // Send the response
  request->send(response);
}

void EspWebUI::setupRoutes() {

  server.on("/login", HTTP_GET, [this](AsyncWebServerRequest *request) {
    sendGzipChunkedResponse(request, gzip_login_html, gzip_login_html_size, "text/html", false, WEBUI_CHUNK_SIZE);
  });

  server.on("/close_all_ws_clients", HTTP_POST, [this](AsyncWebServerRequest *request) {
    if (!isAuthenticated(request)) {
      request->send(401, "text/plain", "authentication required");
      return;
    }

    ws.cleanupClients();
    // ws.closeAll(); // causes crash with EspAsyncWebServer 3.3.22
    request->send(200, "application/json", "{\"status\":\"all clients closed\"}");
  });

  server.on("/login", HTTP_POST, [this](AsyncWebServerRequest *request) {
    if (request->hasParam("username", true) && request->hasParam("password", true)) {
      String username = request->getParam("username", true)->value();
      String password = request->getParam("password", true)->value();

      bool credentialsConfigured = (config.username[0] != '\0' && config.password[0] != '\0');
      bool usernameValid = credentialsConfigured && (username == String(config.username));
      bool passwordValid = credentialsConfigured && (password == String(config.password));

      AsyncResponseStream *response = request->beginResponseStream("application/json");

      if (usernameValid && passwordValid) {
        generateSessionToken(sessionToken, sizeof(sessionToken));
        char cookieHeader[128];
        snprintf(cookieHeader, sizeof(cookieHeader), "esp_jaro_auth=%s; Path=/; HttpOnly; SameSite=Strict; Max-Age=3600", sessionToken);
        response->addHeader("Set-Cookie", cookieHeader);

        response->print("{\"success\": true}");
      } else {
        response->print("{\"success\": false}");
      }

      request->send(response);
    } else {
      request->send(400, "application/json", "{\"success\": false, \"error\": \"missing_parameters\"}");
    }
  });

  server.on("/logout", HTTP_GET, [this](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(303); // 303 See Other
    response->addHeader("Location", "/login");
    // sets the expiration date of the cookie to a time in the past to delete it
    char cookieHeader[128];
    snprintf(cookieHeader, sizeof(cookieHeader), "%s; Path=/; HttpOnly; Expires=Thu, 01 Jan 1970 00:00:00 GMT; Max-Age=0", cookieName);
    response->addHeader("Set-Cookie", cookieHeader);
    request->send(response);
  });

  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    sendGzipChunkedResponse(request, gzip_user_html, gzip_user_html_size, "text/html", true, WEBUI_CHUNK_SIZE);
  });

  server.on("/lib.css", HTTP_GET, [this](AsyncWebServerRequest *request) {
    sendGzipChunkedResponse(request, gzip_lib_css, gzip_lib_css_size, "text/css", false, WEBUI_CHUNK_SIZE);
  });

  server.on("/user.css", HTTP_GET, [this](AsyncWebServerRequest *request) {
    sendGzipChunkedResponse(request, gzip_user_css, gzip_user_css_size, "text/css", false, WEBUI_CHUNK_SIZE);
  });

  server.on("/lib.js", HTTP_GET, [this](AsyncWebServerRequest *request) {
    sendGzipChunkedResponse(request, gzip_lib_js, gzip_lib_js_size, "text/js", false, WEBUI_CHUNK_SIZE);
  });

  server.on("/user.js", HTTP_GET, [this](AsyncWebServerRequest *request) {
    sendGzipChunkedResponse(request, gzip_user_js, gzip_user_js_size, "text/js", false, WEBUI_CHUNK_SIZE);
  });

  server.on("/gzip_ntp", HTTP_GET, [this](AsyncWebServerRequest *request) {
    sendGzipChunkedResponse(request, gzip_ntp_html, gzip_ntp_html_size, "text/html", false, WEBUI_CHUNK_SIZE);
  });

  server.on("/favicon.svg", HTTP_GET, [this](AsyncWebServerRequest *request) { request->send(200, "image/svg+xml", faviconSvg); });

  // config.json download
  server.on("/config-download", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!isAuthenticated(request)) {
      request->send(401, "text/plain", "authentication required");
      return;
    }
    request->send(LittleFS, "/config.json", "application/octet-stream");
  });

  // send config.json file
  server.on("/config.json", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!isAuthenticated(request)) {
      request->send(401, "text/plain", "authentication required");
      return;
    }
    request->send(LittleFS, "/config.json", "application/json");
  });

  // config.json upload
  server.on(
      "/config-upload", HTTP_POST,
      [this](AsyncWebServerRequest *request) {
        if (!isAuthenticated(request)) {
          request->send(401, "text/plain", "authentication required");
          return;
        }

        const long uploadStatus = request->getAttribute(CONFIG_UPLOAD_STATUS_ATTRIBUTE, 500L);
        if (uploadStatus == 200) {
          request->send(200, "text/plain", "upload done!");
        } else if (uploadStatus == 400) {
          request->send(400, "text/plain", "invalid JSON!");
        } else if (uploadStatus == 409) {
          request->send(409, "text/plain", "config upload already in progress");
        } else {
          request->send(500, "text/plain", "upload failed!");
        }
      },
      [this](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!isAuthenticated(request)) {
          return;
        }

        auto uploadError = [this, request](long httpStatus, const char *message) {
          if (activeConfigUploadRequest == request) {
            request->_tempFile.close();
            LittleFS.remove(CONFIG_UPLOAD_TEMP_FILENAME);
            activeConfigUploadRequest = nullptr;
          }

          request->setAttribute(CONFIG_UPLOAD_STATUS_ATTRIBUTE, httpStatus);
          callbackUpload(UPLOAD_ERROR, message);
          ESP_LOGE(TAG, "%s", message);
        };

        if (!index) { // first call for upload
          if (activeConfigUploadRequest != nullptr && activeConfigUploadRequest != request) {
            request->setAttribute(CONFIG_UPLOAD_STATUS_ATTRIBUTE, 409L);
            ESP_LOGW(TAG, "Config upload rejected: another upload is already active");
            return;
          }

          activeConfigUploadRequest = request;
          request->setAttribute(CONFIG_UPLOAD_STATUS_ATTRIBUTE, 0L);

          request->onDisconnect([this, request]() {
            if (activeConfigUploadRequest == request) {
              ESP_LOGW(TAG, "Config upload client disconnected");
              request->_tempFile.close();
              LittleFS.remove(CONFIG_UPLOAD_TEMP_FILENAME);
              activeConfigUploadRequest = nullptr;
              callbackUpload(UPLOAD_ERROR, "config upload aborted: client disconnected");
            }
          });

          callbackUpload(UPLOAD_BEGIN, "uploading...");
          ESP_LOGI(TAG, "Upload Start: %s\n", filename.c_str());

          LittleFS.remove(CONFIG_UPLOAD_TEMP_FILENAME);
          request->_tempFile = LittleFS.open(CONFIG_UPLOAD_TEMP_FILENAME, "w");
          if (!request->_tempFile) {
            uploadError(500L, "error creating upload file!");
            return;
          }
        }

        if (request->getAttribute(CONFIG_UPLOAD_STATUS_ATTRIBUTE, 500L) != 0L) {
          return;
        }

        if (activeConfigUploadRequest != request) {
          request->setAttribute(CONFIG_UPLOAD_STATUS_ATTRIBUTE, 409L);
          ESP_LOGW(TAG, "Config upload rejected: request does not own upload session");
          return;
        }

        if (len && request->_tempFile.write(data, len) != len) {
          uploadError(500L, "error writing upload file!");
          return;
        }

        if (final) {
          request->_tempFile.close();

          File uploadedConfig = LittleFS.open(CONFIG_UPLOAD_TEMP_FILENAME, "r");
          if (!uploadedConfig) {
            uploadError(500L, "error reading upload file!");
            return;
          }

          JsonDocument doc;
          DeserializationError error = deserializeJson(doc, uploadedConfig);
          uploadedConfig.close();

          if (error) {
            uploadError(400L, "invalid JSON!");
            return;
          }

          if (!LittleFS.rename(CONFIG_UPLOAD_TEMP_FILENAME, "/config.json")) {
            uploadError(500L, "error replacing config file!");
            return;
          }

          request->setAttribute(CONFIG_UPLOAD_STATUS_ATTRIBUTE, 200L);
          ESP_LOGI(TAG, "UploadEnd: %s, %u B\n", filename.c_str(), index + len);
          callbackUpload(UPLOAD_FINISH, "upload done!");
          activeConfigUploadRequest = nullptr;
        }
      });

  // Route für OTA-Updates
  server.on(
      "/update", HTTP_POST,
      [this](AsyncWebServerRequest *request) {
        if (!isAuthenticated(request)) {
          request->send(401, "text/plain", "authentication required");
        }
      },
      [this](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!isAuthenticated(request)) {
          return;
        }

        this->handleDoUpdate(request, filename, index, data, len, final);
      });
}
