#include <WiFi.h>
#include <basics.h>
#include <jarolift.h>
#include <language.h>
#include <message.h>
#include <mqtt.h>
#include <mqttDiscovery.h>
#include <atomic>
#include <freertos/semphr.h>
#include <queue>

#define MAX_MQTT_CMD 20

/* D E C L A R A T I O N S ****************************************************/
#define PAYLOAD_LEN 512
struct s_MqttMessage {
  char topic[512];
  char payload[PAYLOAD_LEN];
  int len;
};

std::queue<s_MqttMessage> mqttCmdQueue;
static void processMqttMessage(const s_MqttMessage &msgCpy);
static AsyncMqttClient mqtt_client;
static SemaphoreHandle_t mqttCmdQueueMutex = xSemaphoreCreateMutex();
static bool bootUpMsgDone, setupDone = false;
static bool callbacksRegistered = false;
static const char *TAG = "MQTT"; // LOG TAG
static char lastError[64] = "---";
static char mqttWillTopic[256];
static bool mqttConnectAttempted = false;
static unsigned long lastMqttConnectAttempt = 0;
static std::atomic<bool> mqttNetworkReconnectRequested{false};

/**
 * *******************************************************************
 * @brief   add message to mqtt command buffer
 * @param   topic, payload, len
 * @return  none
 * *******************************************************************/
void addMqttCmd(const char *topic, const char *payload, int len) {
  s_MqttMessage message;
  strncpy(message.topic, topic, sizeof(message.topic) - 1);
  message.topic[sizeof(message.topic) - 1] = '\0';

  strncpy(message.payload, payload, sizeof(message.payload) - 1);
  message.payload[sizeof(message.payload) - 1] = '\0';

  message.len = len;

  bool commandQueued = false;
  bool mqttEnabled = false;

  xSemaphoreTake(mqttCmdQueueMutex, portMAX_DELAY);

  mqttEnabled = config.mqtt.enable;

  if (mqttEnabled && mqttCmdQueue.size() < MAX_MQTT_CMD) {
      mqttCmdQueue.push(message);
      commandQueued = true;
  }

  xSemaphoreGive(mqttCmdQueueMutex);

  if (commandQueued) {
      ESP_LOGD(TAG, "add msg to buffer: %s, %s", topic, payload);
  } else if (mqttEnabled) {
      ESP_LOGE(TAG, "too many commands within too short time");
  } else {
      ESP_LOGD(TAG, "MQTT command ignored while MQTT is disabled");
  }
}

/**
 * *******************************************************************
 * @brief   mqtt publish wrapper
 * @param   topic, payload, retained
 * @return  none
 * *******************************************************************/
void mqttPublish(const char *topic, const char *payload, boolean retained) { mqtt_client.publish(topic, 0, retained, payload); }

/**
 * *******************************************************************
 * @brief   publish enable state of one schedule
 * @param   schedule number 1..6
 * @return  none
 * *******************************************************************/
static void mqttPublishScheduleEnable(uint8_t schedule) {

  if (schedule < 1 || schedule > 6) {
    return;
  }

  char suffix[48];
  snprintf(suffix, sizeof(suffix), "/status/schedule/%u/enable", schedule);

  mqttPublish(
      addTopic(suffix),
      config.timer[schedule - 1].enable ? "true" : "false",
      true);
}

/**
 * *******************************************************************
 * @brief   helper function to add subject to mqtt topic
 * @param   none
 * @return  none
 * *******************************************************************/
const char *addTopic(const char *suffix) {
  static char newTopic[256];
  snprintf(newTopic, sizeof(newTopic), "%s%s", config.mqtt.topic, suffix);
  return newTopic;
}

/**
 * *******************************************************************
 * @brief   helper function to add subject to mqtt topic
 * @param   none
 * @return  none
 * *******************************************************************/
const char *addCfgCmdTopic(const char *suffix) {
  static char newTopic[256];
  snprintf(newTopic, sizeof(newTopic), "%s/setvalue/%s", config.mqtt.topic, suffix);
  return newTopic;
}

/**
 * *******************************************************************
 * @brief   MQTT callback function for incoming message
 * @param   topic, payload
 * @return  none
 * *******************************************************************/
void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {

  s_MqttMessage msgCpy{};

  if (index != 0 || len != total) {
    ESP_LOGW(TAG,
             "fragmented MQTT payload ignored: index=%u len=%u total=%u",
             index, len, total);
    return;
  }

  if (len >= PAYLOAD_LEN) {
    ESP_LOGW(TAG, "MQTT payload too large: %u bytes", len);
    return;
  }

  msgCpy.len = len;

  if (topic == NULL) {
    msgCpy.topic[0] = '\0';
  } else {
    strncpy(msgCpy.topic, topic, sizeof(msgCpy.topic) - 1);
    msgCpy.topic[sizeof(msgCpy.topic) - 1] = '\0';
  }
  if (payload == NULL) {
    msgCpy.payload[0] = '\0';
  } else if (len > 0 && len < PAYLOAD_LEN) {
    memcpy(msgCpy.payload, payload, len);
    msgCpy.payload[len] = '\0';
  }

  addMqttCmd(msgCpy.topic, msgCpy.payload, msgCpy.len);

  ESP_LOGI(TAG, "msg received | topic: %s | payload: %s", msgCpy.topic, msgCpy.payload);
}

/**
 * *******************************************************************
 * @brief   callback function if MQTT gets connected
 * @param   none
 * @return  none
 * *******************************************************************/
void onMqttConnect(bool sessionPresent) {
  snprintf(lastError, sizeof(lastError), "---");
  ESP_LOGI(TAG, "MQTT connected");
  // Once connected, publish an announcement...
  sendWiFiInfo();
  // ... and resubscribe
  mqtt_client.subscribe(addTopic("/cmd/#"), 0);
  mqtt_client.subscribe(addTopic("/setvalue/#"), 0);
  mqtt_client.subscribe("homeassistant/status", 0);
  for (uint8_t schedule = 1; schedule <= 6; schedule++) {
    mqttPublishScheduleEnable(schedule);
  }
}

/**
 * *******************************************************************
 * @brief   callback function if MQTT gets disconnected
 * @param   none
 * @return  none
 * *******************************************************************/
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {

  switch (reason) {
  case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
    snprintf(lastError, sizeof(lastError), "TCP DISCONNECTED");
    break;
  case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
    snprintf(lastError, sizeof(lastError), "MQTT UNACCEPTABLE PROTOCOL VERSION");
    break;
  case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
    snprintf(lastError, sizeof(lastError), "MQTT IDENTIFIER REJECTED");
    break;
  case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
    snprintf(lastError, sizeof(lastError), "MQTT SERVER UNAVAILABLE");
    break;
  case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
    snprintf(lastError, sizeof(lastError), "MQTT MALFORMED CREDENTIALS");
    break;
  case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
    snprintf(lastError, sizeof(lastError), "MQTT NOT AUTHORIZED");
    break;
  case AsyncMqttClientDisconnectReason::TLS_BAD_FINGERPRINT:
    snprintf(lastError, sizeof(lastError), "TLS BAD FINGERPRINT");
    break;
  default:
    snprintf(lastError, sizeof(lastError), "UNKNOWN ERROR");
    break;
  }

  ESP_LOGW(TAG, "MQTT disconnected | reason: %s | WiFi: %s | Ethernet: %s | connected: %s", lastError, wifi.connected ? "yes" : "no",
           eth.connected ? "yes" : "no", mqtt_client.connected() ? "yes" : "no");
}

/**
 * *******************************************************************
 * @brief   is MQTT connected
 * @param   none
 * @return  none
 * *******************************************************************/
bool mqttIsConnected() { return mqtt_client.connected(); }

const char *mqttGetLastError() { return lastError; }

void mqttReconnectForNetworkChange() {
  if (config.mqtt.enable) {
    mqttNetworkReconnectRequested.store(true);
  }
}

/**
 * *******************************************************************
 * @brief   Basic MQTT setup
 * @param   none
 * @return  none
 * *******************************************************************/
void mqttSetup() {

  if (!callbacksRegistered) {
    mqtt_client.onConnect(onMqttConnect);
    mqtt_client.onDisconnect(onMqttDisconnect);
    mqtt_client.onMessage(onMqttMessage);
    callbacksRegistered = true;
  }
  mqtt_client.setServer(config.mqtt.server, config.mqtt.port);
  mqtt_client.setClientId(config.wifi.hostname);
  mqtt_client.setCredentials(config.mqtt.user, config.mqtt.password);
  snprintf(mqttWillTopic, sizeof(mqttWillTopic), "%s/status", config.mqtt.topic);
  mqtt_client.setWill(mqttWillTopic, 0, true, "offline");
  mqtt_client.setKeepAlive(10);

  ESP_LOGI(TAG, "MQTT setup done!");
}

/**
 * *******************************************************************
 * @brief   enable or disable MQTT
 * @param   enabled
 * @return  none
 * *******************************************************************/
void mqttSetEnabled(bool enabled) {
    if (enabled && config.mqtt.enable) {
        return;
    }

    xSemaphoreTake(mqttCmdQueueMutex, portMAX_DELAY);

    config.mqtt.enable = enabled;

    if (!enabled) {
        while (!mqttCmdQueue.empty()) {
            mqttCmdQueue.pop();
        }
    }

    xSemaphoreGive(mqttCmdQueueMutex);

    if (!enabled) {
        if (mqtt_client.connected()) {
            mqtt_client.disconnect();
        } else {
            mqtt_client.disconnect(true);
        }
    }

    setupDone = false;
}

/**
 * *******************************************************************
 * @brief   MQTT cyclic function
 * @param   none
 * @return  none
 * *******************************************************************/
void mqttCyclic() {

  // process incoming messages
  s_MqttMessage mqttMessage;
  bool hasMqttMessage = false;
  xSemaphoreTake(mqttCmdQueueMutex, portMAX_DELAY);
  if (!mqttCmdQueue.empty()) {
    mqttMessage = mqttCmdQueue.front();
    mqttCmdQueue.pop();
    hasMqttMessage = true;
  }
  xSemaphoreGive(mqttCmdQueueMutex);

  if (hasMqttMessage) {
    processMqttMessage(mqttMessage);
  }

  if (mqttNetworkReconnectRequested.exchange(false)) {
    mqtt_client.disconnect(true);
    mqttConnectAttempted = false;
  }

  // call setup when connection is established
  if (config.mqtt.enable && !setupMode && !setupDone && (eth.connected || wifi.connected)) {
    mqttSetup();
    setupDone = true;
    mqttConnectAttempted = false;
  }

  // automatic reconnect to mqtt broker if connection is lost
  if (mqtt_client.connected()) {
    mqttConnectAttempted = false;
  } else if (config.mqtt.enable && (wifi.connected || eth.connected)) {
    const unsigned long now = millis();
    if (!mqttConnectAttempted || now - lastMqttConnectAttempt >= MQTT_RECONNECT) {
      const bool retry = mqttConnectAttempted;
      mqttConnectAttempted = true;
      lastMqttConnectAttempt = now;
      ESP_LOGI(TAG, "MQTT connection attempt | retry: %s | WiFi: %s | Ethernet: %s", retry ? "yes" : "no", wifi.connected ? "yes" : "no",
               eth.connected ? "yes" : "no");
      mqtt_client.connect();
    }
  }

  // send bootup messages after restart and established mqtt connection
  if (!bootUpMsgDone && mqtt_client.connected()) {
    bootUpMsgDone = true;
    ESP_LOGI(TAG, "ESP restarted (%s)", EspSysUtil::RestartReason::get());

    if (config.mqtt.ha_enable) {
      mqttDiscoverySetup(false);
    }
  }
}

/**
 * *******************************************************************
 * @brief   helper function to check shutter commands
 * @param   topicCopy, cmpTopic
 * @return  none
 * *******************************************************************/
int checkJaroCmd(const char *topicCopy, const char *cmpTopic, int maxChannel) {

  size_t cmpTopicLen = strlen(cmpTopic);

  if (strncmp(topicCopy, cmpTopic, cmpTopicLen) == 0) {
    const char *suffix = topicCopy + cmpTopicLen;
    char *endPtr;
    int channel = strtol(suffix, &endPtr, 10);

    if (*endPtr == '\0' && channel >= 1 && channel <= maxChannel) {
      return channel;
    }
  }
  return -1;
}

/**
 * *******************************************************************
 * @brief   check schedule enable command topic
 * @param   topic
 * @return  schedule number 1..6, otherwise -1
 * *******************************************************************/
static int checkScheduleEnableCmd(const char *topic) {

  const char *baseTopic = addCfgCmdTopic("schedule/");
  size_t baseTopicLen = strlen(baseTopic);

  if (strncmp(topic, baseTopic, baseTopicLen) != 0) {
    return -1;
  }

  const char *suffix = topic + baseTopicLen;

  if (suffix[0] < '1' || suffix[0] > '6') {
    return -1;
  }

  if (strcmp(suffix + 1, "/enable") != 0) {
    return -1;
  }

  return suffix[0] - '0';
}

/**
 * *******************************************************************
 * @brief  parseMask:
 * @details - Removes any whitespace characters
 *  - Converts everything to lowercase
 *  - Determines the format based on prefix:
 *      "0x" => hexadecimal
 *      "0b" => binary
 *      otherwise => decimal
 * @param   topicCopy, cmpTopic
 * @return  the interpreted number as a uint16_t
 * *******************************************************************/

uint16_t parseMask(const char *payload) {
  char buffer[64];
  int idx = 0;

  // 1) Remove whitespaces and convert to lowercase
  while (*payload != '\0' && idx < (int)(sizeof(buffer) - 1)) {
    if (!isspace((unsigned char)*payload)) {
      buffer[idx++] = (char)tolower((unsigned char)*payload);
    }
    payload++;
  }
  buffer[idx] = '\0'; // null termination for the new string

  // 2) Detect prefix
  // "0x" => hexadecimal
  if (strncmp(buffer, "0x", 2) == 0) {
    // strtol parses the substring after "0x" in base 16
    return (uint16_t)strtol(buffer + 2, NULL, 16);
  }
  // "0b" => binary
  else if (strncmp(buffer, "0b", 2) == 0) {
    return (uint16_t)strtol(buffer + 2, NULL, 2);
  }
  // otherwise => decimal
  else {
    return (uint16_t)strtol(buffer, NULL, 10);
  }
}

/**
 * *******************************************************************
 * @brief   MQTT callback function for incoming message
 * @param   topic, payload
 * @return  none
 * *******************************************************************/
static void processMqttMessage(const s_MqttMessage &msgCpy) {

  ESP_LOGD(TAG, "process msg from buffer: %s, %s", msgCpy.topic, msgCpy.payload);

  // payload as number
  // msgCpy.intVal = 0;
  // msgCpy.floatVal = 0.0;
  // if (len > 0) {
  //   msgCpy.intVal = atoi(msgCpy.payload);
  //   msgCpy.floatVal = atoff(msgCpy.payload);
  // }

  const char *shutterTopic = addTopic("/cmd/shutter/");
  int channel = checkJaroCmd(msgCpy.topic, shutterTopic, 16);
  const char *groupTopic = addTopic("/cmd/group/");
  int group = checkJaroCmd(msgCpy.topic, groupTopic, 6);
  int schedule = checkScheduleEnableCmd(msgCpy.topic);

  ESP_LOGD(TAG, "channel: %i", channel);
  ESP_LOGD(TAG, "group: %i", group);
  ESP_LOGD(TAG, "schedule: %i", schedule);

  // restart ESP command
  if (strcasecmp(msgCpy.topic, addTopic("/cmd/restart")) == 0) {
    EspSysUtil::RestartReason::saveLocal("mqtt command");
    yield();
    delay(1000);
    yield();
    ESP.restart();
    // reconfigure
  } else if (strcasecmp(msgCpy.topic, addTopic("/cmd/reconfigure")) == 0) {
    mqttDiscoverySetup(true);
    yield();
    delay(1000);
    yield();
    mqttDiscoverySetup(false);
    // homeassistant/status
  } else if (strcmp(msgCpy.topic, "homeassistant/status") == 0) {
    if (config.mqtt.ha_enable && strcmp(msgCpy.payload, "online") == 0) {
      mqttDiscoverySetup(false); // send actual discovery configuration
    }
    // schedule enable command
  } else if (schedule != -1) {

    bool enabled = false;
    bool validPayload = true;

    if (strcasecmp(msgCpy.payload, "true") == 0) {
      enabled = true;
    } else if (strcasecmp(msgCpy.payload, "false") == 0) {
      enabled = false;
    } else {
      validPayload = false;
    }

    if (!validPayload) {
      mqttPublish(addTopic("/message"), "invalid schedule enable value", false);
      ESP_LOGW(TAG, "invalid schedule %i enable value: %s", schedule, msgCpy.payload);
    } else {
      if (config.timer[schedule - 1].enable != enabled) {
        config.timer[schedule - 1].enable = enabled;

        ESP_LOGI(TAG,
                 "schedule %i %s",
                 schedule,
                 enabled ? "enabled" : "disabled");
      }

      mqttPublishScheduleEnable(schedule);
    }
    // Shutter commands
  } else if (channel != -1) {
    if (channel >= 1 && channel <= 16) {
      if (strcasecmp(msgCpy.payload, "UP") == 0 || strcasecmp(msgCpy.payload, "OPEN") == 0 || strcmp(msgCpy.payload, "0") == 0) {
        jaroCmd(CMD_UP, channel - 1);
      } else if (strcasecmp(msgCpy.payload, "DOWN") == 0 || strcasecmp(msgCpy.payload, "CLOSE") == 0 || strcmp(msgCpy.payload, "1") == 0) {
        jaroCmd(CMD_DOWN, channel - 1);
      } else if (strcasecmp(msgCpy.payload, "STOP") == 0 || strcmp(msgCpy.payload, "2") == 0) {
        jaroCmd(CMD_STOP, channel - 1);
      } else if (strcasecmp(msgCpy.payload, "SHADE") == 0 || strcmp(msgCpy.payload, "3") == 0) {
        jaroCmd(CMD_SHADE, channel - 1);
      } else if (strcasecmp(msgCpy.payload, "SETSHADE") == 0 || strcmp(msgCpy.payload, "4") == 0) {
        jaroCmd(CMD_SET_SHADE, channel - 1);
      } else {
        mqttPublish(addTopic("/message"), "invalid shutter cmd", false);
        ESP_LOGW(TAG, "invalid shutter cmd");
      }
    } else {
      mqttPublish(addTopic("/message"), "invalid channel", false);
      ESP_LOGW(TAG, "invalid channel for shutter cmd");
    }
    // Group commands
  } else if (group != -1) {
    if (group >= 1 && group <= 6) {
      if (strcasecmp(msgCpy.payload, "UP") == 0 || strcasecmp(msgCpy.payload, "OPEN") == 0 || strcmp(msgCpy.payload, "0") == 0) {
        jaroCmd(CMD_GRP_UP, config.jaro.grp_mask[group - 1]);
      } else if (strcasecmp(msgCpy.payload, "DOWN") == 0 || strcasecmp(msgCpy.payload, "CLOSE") == 0 || strcmp(msgCpy.payload, "1") == 0) {
        jaroCmd(CMD_GRP_DOWN, config.jaro.grp_mask[group - 1]);
      } else if (strcasecmp(msgCpy.payload, "STOP") == 0 || strcmp(msgCpy.payload, "2") == 0) {
        jaroCmd(CMD_GRP_STOP, config.jaro.grp_mask[group - 1]);
      } else if (strcasecmp(msgCpy.payload, "SHADE") == 0 || strcmp(msgCpy.payload, "3") == 0) {
        jaroCmd(CMD_GRP_SHADE, config.jaro.grp_mask[group - 1]);
      } else {
        mqttPublish(addTopic("/message"), "invalid group cmd", false);
        ESP_LOGW(TAG, "invalid group cmd");
      }
    } else {
      mqttPublish(addTopic("/message"), "invalid group", false);
      ESP_LOGW(TAG, "invalid channel for group cmd");
    }
    // Group commands with bitmask
  } else if (strcasecmp(msgCpy.topic, addTopic("/cmd/group/up")) == 0) {
    jaroCmd(CMD_GRP_UP, parseMask(msgCpy.payload));
  } else if (strcasecmp(msgCpy.topic, addTopic("/cmd/group/down")) == 0) {
    jaroCmd(CMD_GRP_DOWN, parseMask(msgCpy.payload));
  } else if (strcasecmp(msgCpy.topic, addTopic("/cmd/group/stop")) == 0) {
    jaroCmd(CMD_GRP_STOP, parseMask(msgCpy.payload));
  } else if (strcasecmp(msgCpy.topic, addTopic("/cmd/group/shade")) == 0) {
    jaroCmd(CMD_GRP_SHADE, parseMask(msgCpy.payload));
  } else {
    mqttPublish(addTopic("/message"), "unknown topic", false);
    ESP_LOGI(TAG, "unknown topic received");
  }

}
