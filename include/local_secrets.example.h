#pragma once

// WPA2 password for the temporary setup access point.
// Use 8 to 63 characters.
#define SETUP_AP_PASSWORD "change-this-password"

// Unique 16-byte AES key for local configuration encryption.
// Generate your own random bytes. Do not commit the real key.
static constexpr unsigned char CONFIG_ENCRYPTION_KEY[16] = {
  0x00, 0x11, 0x22, 0x33,
  0x44, 0x55, 0x66, 0x77,
  0x88, 0x99, 0xaa, 0xbb,
  0xcc, 0xdd, 0xee, 0xff
};
