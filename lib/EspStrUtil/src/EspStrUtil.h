#pragma once

#include <Arduino.h>
#include <cctype>
#include <cstring>
#include <mbedtls/aes.h>
#include <mbedtls/base64.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

class EspStrUtil {

private:
  static constexpr size_t AES_BLOCK_SIZE = 16;

  // add PKCS7-Padding
  static size_t addPadding(unsigned char *buffer, size_t dataLength,
                           size_t bufferSize) {
    size_t paddingLength = AES_BLOCK_SIZE - (dataLength % AES_BLOCK_SIZE);
    if (dataLength + paddingLength > bufferSize) {
      return 0; // error buffer to small
    }
    memset(buffer + dataLength, paddingLength, paddingLength);
    return dataLength + paddingLength;
  }

  // remove PKCS7-Padding
  static size_t removePadding(unsigned char *buffer, size_t dataLength) {
    if (dataLength == 0 || dataLength % AES_BLOCK_SIZE != 0) {
      return 0; // error: invalid length
    }
    size_t paddingLength = buffer[dataLength - 1];
    if (paddingLength > AES_BLOCK_SIZE || paddingLength > dataLength) {
      return 0; // error: invalid padding
    }
    return dataLength - paddingLength;
  }

public:
  /**
   * *******************************************************************
   * @brief   Encrypts a password and encodes it in Base64 format
   * @param   input         Plaintext password to be encrypted
   * @param   key           16-byte AES key used for encryption
   * @param   output        Buffer to store the Base64-encoded, encrypted
   * password
   * @param   maxOutputSize Maximum size of the output buffer
   * @return  true if encryption and Base64 encoding is successful
   * *******************************************************************/
  static bool encryptPassword(const char *input, const unsigned char *key,
                              char *output, size_t maxOutputSize) {
    if (!input || !key || !output || maxOutputSize == 0)
      return false;

    unsigned char paddedInput[128 + AES_BLOCK_SIZE] = {0};
    unsigned char encrypted[128 + AES_BLOCK_SIZE] = {0};

    size_t inputLength = strlen(input);
    if (inputLength > 128)
      return false; // Password to long

    memcpy(paddedInput, input, inputLength);
    size_t paddedLength =
        addPadding(paddedInput, inputLength, sizeof(paddedInput));
    if (paddedLength == 0)
      return false; // Padding-error

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, key, 128);

    for (size_t i = 0; i < paddedLength; i += AES_BLOCK_SIZE) {
      mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, paddedInput + i,
                            encrypted + i);
    }
    mbedtls_aes_free(&aes);

    size_t base64Length = 0;
    if (mbedtls_base64_encode((unsigned char *)output, maxOutputSize,
                              &base64Length, encrypted, paddedLength) != 0) {
      return false;
    }

    output[base64Length] = '\0';
    return true;
  }

  /**
   * *******************************************************************
   * @brief   Decrypts a Base64-encoded, encrypted password
   * @param   input         Base64-encoded encrypted password
   * @param   key           16-byte AES key used for decryption
   * @param   output        Buffer to store the decrypted password
   * @param   maxOutputSize Maximum size of the output buffer
   * @return  true if decryption and padding removal were successful
   * *******************************************************************/
  static bool decryptPassword(const char *input, const unsigned char *key,
                              char *output, size_t maxOutputSize) {
    if (!input || !key || !output || maxOutputSize == 0)
      return false;

    unsigned char encrypted[128 + AES_BLOCK_SIZE] = {0};
    unsigned char decrypted[128 + AES_BLOCK_SIZE] = {0};

    size_t encryptedLength = sizeof(encrypted);
    if (mbedtls_base64_decode(encrypted, encryptedLength, &encryptedLength,
                              (const unsigned char *)input,
                              strlen(input)) != 0) {
      return false; // error Base64-decoding
    }

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, key, 128);

    for (size_t i = 0; i < encryptedLength; i += AES_BLOCK_SIZE) {
      mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, encrypted + i,
                            decrypted + i);
    }
    mbedtls_aes_free(&aes);

    size_t actualLength = removePadding(decrypted, encryptedLength);
    if (actualLength == 0 || actualLength >= maxOutputSize) {
      return false; // error
    }

    memcpy(output, decrypted, actualLength);
    output[actualLength] = '\0';
    return true;
  }

  /**
   * *******************************************************************
   * @brief   create String from integer
   * @param   value as int8_t
   * @return  pointer to char array - pay attention, it is local static
   * *******************************************************************/
  static inline const char *intToString(int8_t value) {
    static char ret_str[5];
    snprintf(ret_str, sizeof(ret_str), "%" PRId8, value);
    return ret_str;
  }

  /**
   * *******************************************************************
   * @brief   create String from integer
   * @param   value as uint8_t
   * @return  pointer to char array - pay attention, it is local static
   * *******************************************************************/
  static inline const char *intToString(uint8_t value) {
    static char ret_str[5];
    snprintf(ret_str, sizeof(ret_str), "%" PRIu8, value);
    return ret_str;
  }

  /**
   * *******************************************************************
   * @brief   create String from integer
   * @param   value as uint16_t
   * @return  pointer to char array - pay attention, it is local static
   * *******************************************************************/
  static inline const char *intToString(uint16_t value) {
    static char ret_str[10];
    snprintf(ret_str, sizeof(ret_str), "%" PRId16, value);
    return ret_str;
  }

  /**
   * *******************************************************************
   * @brief   create String from integer
   * @param   value as int
   * @return  pointer to char array - pay attention, it is local static
   * *******************************************************************/
  static inline const char *intToString(int value) {
    static char ret_str[10];
    snprintf(ret_str, sizeof(ret_str), "%" PRIi16, value);
    return ret_str;
  }

  /**
   * *******************************************************************
   * @brief   create String from integer
   * @param   value as int32_t
   * @return  pointer to char array - pay attention, it is local static
   * *******************************************************************/
  static inline const char *intToString(int32_t value) {
    static char ret_str[64];
    snprintf(ret_str, sizeof(ret_str), "%" PRId32, value);
    return ret_str;
  }

  /**
   * *******************************************************************
   * @brief   create String from integer
   * @param   value as int64_t
   * @return  pointer to char array - pay attention, it is local static
   * *******************************************************************/
  static inline const char *intToString(uint64_t value) {
    static char ret_str[21];
    char *ptr = ret_str;
    do {
      *ptr++ = (value % 10) + '0';
      value /= 10;
    } while (value != 0);
    *ptr-- = '\0';
    char *start_ptr = ret_str;
    char tmp_char;
    while (start_ptr < ptr) {
      tmp_char = *start_ptr;
      *start_ptr++ = *ptr;
      *ptr-- = tmp_char;
    }
    return ret_str;
  }

  /**
   * *******************************************************************
   * @brief   create String from integer
   * @param   value as intmax_t
   * @return  pointer to char array - pay attention, it is local static
   * *******************************************************************/
  static inline const char *intToString(intmax_t value) {
    static char ret_str[64];
    snprintf(ret_str, sizeof(ret_str), "%" PRIdMAX, value);
    return ret_str;
  }

  /**
   * *******************************************************************
   * @brief   Combines all float and double conversions with custom decimal
   * precision
   * @param   float or double value
   * @param   decimal precision
   * @return  string representation of the float or double value
   * *******************************************************************/
  template <typename T>
  static inline const char *floatToString(T value, int decimals = 1) {
    static char ret_str[64];
    char format[8];
    snprintf(format, sizeof(format), "%%.%df", decimals);
    snprintf(ret_str, sizeof(ret_str), format, value);
    return ret_str;
  }

  /**
   * *******************************************************************
   * @brief   safe strcat function
   * @param   dest string destination
   * @param   src string source
   * @param   dest_size destination size
   * @return  none
   * *******************************************************************/
  static inline char *strcat_safe(char *dest, const char *src,
                                  size_t dest_size) {
    size_t dest_len = strlen(dest);
    size_t src_len = strlen(src);

    if (dest_len + src_len >= dest_size) {
      // not enough space
      return NULL;
    }
    strcat(dest, src);
    return dest;
  }

  /**
   * *******************************************************************
   * @brief   convert String to BOOL if String contains true/false
   * @param   str e
   * @return  true/false
   * *******************************************************************/
  static inline bool stringToBool(const char *str) {
    if (strcmp(str, "true") == 0) {
      return true;
    }
    return false;
  }

  /**
   * *******************************************************************
   * @brief   create date and time as String
   * @param   none
   * @return  pointer to date and time String (local static memory)
   * *******************************************************************/
  static inline const char *getDateTimeString() {
    /* ---------------- INFO ---------------------------------
    dti.tm_year + 1900  // years since 1900
    dti.tm_mon + 1      // January = 0 (!)
    dti.tm_mday         // day of month
    dti.tm_hour         // hours since midnight  0-23
    dti.tm_min          // minutes after the hour  0-59
    dti.tm_sec          // seconds after the minute  0-61*
    dti.tm_wday         // days since Sunday 0-6
    dti.tm_isdst        // Daylight Saving Time flag
    --------------------------------------------------------- */
    static char dateTimeInfo[74] = {'\0'}; // Date and time info String
    time_t now;                            // this is the epoch
    tm dti; // the structure tm holds time information in a more convient way
    time(&now);              // read the current time
    localtime_r(&now, &dti); // update the structure tm with the current time
    snprintf(dateTimeInfo, sizeof(dateTimeInfo),
             "%02d.%02d.%02d - %02i:%02i:%02i", dti.tm_mday, (dti.tm_mon + 1),
             (dti.tm_year + 1900), dti.tm_hour, dti.tm_min, dti.tm_sec);
    return dateTimeInfo;
  }

  /**
   * *******************************************************************
   * @brief   create date  String
   * @param   none
   * @return  pointer to date String (local static memory)
   * *******************************************************************/
  static inline const char *getDateString() {
    static char dateInfo[74] = {'\0'}; // Date String
    time_t now;                        // this is the epoch
    tm dti; // the structure tm holds time information in a more convient way
    time(&now);              // read the current time
    localtime_r(&now, &dti); // update the structure tm with the current time
    snprintf(dateInfo, sizeof(dateInfo), "%02d.%02d.%02d", dti.tm_mday,
             (dti.tm_mon + 1), (dti.tm_year + 1900));
    return dateInfo;
  }
  /**
   * *******************************************************************
   * @brief   create date  String
   * @param   none
   * @return  pointer to date String (local static memory)
   * *******************************************************************/
  static inline const char *getDateStringWeb() {
    static char dateInfo[74] = {'\0'}; // Date String
    time_t now;                        // this is the epoch
    tm dti; // the structure tm holds time information in a more convient way
    time(&now);              // read the current time
    localtime_r(&now, &dti); // update the structure tm with the current time
    snprintf(dateInfo, sizeof(dateInfo), "%02d-%02d-%02d", (dti.tm_year + 1900),
             (dti.tm_mon + 1), dti.tm_mday);
    return dateInfo;
  }

  /**
   * *******************************************************************
   * @brief   create time as String
   * @param   none
   * @return  pointer to time String (local static memory)
   * *******************************************************************/
  static inline const char *getTimeString() {
    static char timeInfo[74] = {'\0'}; // Date and time info String
    time_t now;                        // this is the epoch
    tm dti; // the structure tm holds time information in a more convient way
    time(&now);              // read the current time
    localtime_r(&now, &dti); // update the structure tm with the current time
    snprintf(timeInfo, sizeof(timeInfo), "%02i:%02i:%02i", dti.tm_hour,
             dti.tm_min, dti.tm_sec);
    return timeInfo;
  }

  /**
   * *******************************************************************
   * @brief   format build date/time information
   * @param   date input string
   * @return  hash value
   * *******************************************************************/
  static inline const char *getBuildDateTime() {
    static char formatted_date[32] = {0};
    // Monatsnamen für die Umwandlung in Zahlen
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char month_text[4] = {0};
    int day, year;
    // Extrahiere den Monat, Tag und Jahr aus dem __DATE__ String
    sscanf(__DATE__, "%s %d %d", month_text, &day, &year);

    // Finde den Monat im Array und konvertiere ihn in eine Zahl
    int month = 0;
    for (int i = 0; i < 12; i++) {
      if (strcmp(month_text, months[i]) == 0) {
        month = i + 1;
        break;
      }
    }

    // Format das Datum als "DD-MM-YYYY"
    sprintf(formatted_date, "%02d.%02d.%d - %s", day, month, year, __TIME__);

    return formatted_date;
  }

  /**
   * *******************************************************************
   * @brief   simple hash function
   * @param   str input string
   * @return  hash value
   * *******************************************************************/
  static inline unsigned int strHash(const char *str) {
    unsigned int hash = 0;
    while (*str) {
      hash = 31 * hash + (*str++);
    }
    return hash;
  }

  /**
   * *******************************************************************
   * @brief   helper function to check if a string has changed
   * @param   lastHash
   * @param   currentValue
   * @return  compare result - true if different
   * *******************************************************************/
  static inline bool strDiff(unsigned int *lastHash, const char *currentValue) {
    unsigned int currentHash = strHash(currentValue);
    if (*lastHash != currentHash) {
      *lastHash = currentHash;
      return true;
    }
    return false;
  }

  /**
   * *******************************************************************
   * @brief   helper function to replace underscore with whitespace
   * @param   input string
   * @param   output string
   * @param   output_size
   * @return  none
   * *******************************************************************/
  static inline void replace_underscores(const char *input, char *output,
                                         size_t output_size) {
    if (input == NULL || output == NULL)
      return;
    memset(output, 0, output_size);
    for (size_t i = 0; input[i] != '\0' && i < output_size - 1; i++) {
      output[i] = (input[i] == '_') ? ' ' : input[i];
    }
    output[output_size - 1] = '\0';
  }

  /**
   * *******************************************************************
   * @brief   helper function to replace whitespace with underscore
   * @param   input string
   * @param   output string
   * @param   output_size
   * @return  none
   * *******************************************************************/
  static inline void replace_whitespace(const char *input, char *output,
                                        size_t output_size) {
    if (input == NULL || output == NULL)
      return;
    memset(output, 0, output_size);
    for (size_t i = 0; input[i] != '\0' && i < output_size - 1; i++) {
      output[i] = (input[i] == ' ') ? '_' : input[i];
    }
    output[output_size - 1] = '\0';
  }

  /**
   * *******************************************************************
   * @brief   helper function to lower string input
   * @param   input string
   * @param   output string
   * @param   output_size
   * @return  none
   * *******************************************************************/
  static inline void to_lowercase(const char *input, char *output,
                                  size_t output_size) {
    if (input == NULL || output == NULL)
      return;
    memset(output, 0, output_size);
    size_t i;
    for (i = 0; input[i] != '\0' && i < output_size - 1; i++) {
      output[i] = tolower((unsigned char)input[i]);
    }
    output[i] = '\0';
  }

  /**
   * *******************************************************************
   * @brief   helper function to check if string is a number
   * @param   input string
   * @return  true if string is a number
   * *******************************************************************/
  static inline bool isNumber(const char *str) {
    int num;
    char end;
    if (sscanf(str, "%d%c", &num, &end) == 1) {
      return true; // string is not a number
    } else {
      return false; // string is a number
    }
  }

  /**
   * *******************************************************************
   * @brief   check before read from file
   * @param   dest, size, src
   * @return  none
   * *******************************************************************/
  static inline void readJSONstring(char *dest, size_t size, const char *src) {
    const char *check = src;
    if (check != NULL) {
      snprintf(dest, size, "%s", src);
    }
  }

  // static inline void readJSONstring(std::string &dest,
  //                                   const JsonVariantConst &src,
  //                                   const std::string &defaultValue = "") {
  //   if (src.isNull()) {
  //     dest = defaultValue; // Setze Standardwert
  //   } else {
  //     dest = src.as<std::string>();
  //   }
  // }

  // static inline bool stringToBool(const std::string &str) {
  //   return (str == "true" ||
  //           str == "1"); // "true" oder "1" wird als wahr interpretiert
  // }

  /**
   * *******************************************************************
   * @brief   hash djb2 function
   * @param   str, len
   * @return  none
   * *******************************************************************/
  static inline unsigned long hash(void *str, size_t len) {
    unsigned char *p = (unsigned char *)str;
    unsigned long hash = 5381;
    size_t i;
    for (i = 0; i < len; i++) {
      hash = ((hash << 5) + hash) + p[i]; /* hash * 33 + c */
    }
    return hash;
  }

  /**
   * *******************************************************************
   * @brief   convert Bytes to fromatted string
   * @param   bytes
   * @return  formatted string (Bytes, kBytes, MB, GB)
   * *******************************************************************/
  static inline const char *formatBytes(unsigned long long bytes) {
    const char *units[] = {"Byte", "kB", "MB", "GB"};
    static char formatted[20];
    double value = bytes;
    int unitIndex = 0;

    while (value >= 1024 && unitIndex < 3) {
      value /= 1024.0;
      unitIndex++;
    }

    snprintf(formatted, sizeof(formatted), "%.2f %s", value, units[unitIndex]);
    return formatted;
  }
};