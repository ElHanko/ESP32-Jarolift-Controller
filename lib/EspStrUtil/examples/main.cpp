#include <../src/EspStrUtil.h>
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Hello, World!");
  Serial.println(EspStrUtil::intToString(42));

  // encryption key
  const unsigned char key[16] = {'s', 'e', 'c', 'u', 'r', 'e', '_', 'k', 'e', 'y', '_', 'd', 'y', 'n', 'a', 'm'};

  const char *password = "MySecurePassword123!";
  char encrypted[256] = {0};
  char decrypted[128] = {0};

  // encrypt
  if (EspStrUtil::encryptPassword(password, key, encrypted, sizeof(encrypted))) {
    Serial.println("encrypted password (Base64):");
    Serial.println(encrypted);
  } else {
    Serial.println("error encryption");
  }

  // decrypt
  if (EspStrUtil::decryptPassword(encrypted, key, decrypted, sizeof(decrypted))) {
    Serial.println("decrypted password:");
    Serial.println(decrypted);
  } else {
    Serial.println("error decrypting");
  }
}

void loop() {
  // put your main code here
}