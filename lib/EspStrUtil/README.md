# EspStrUtil

EspStrUtil is a utility library for the ESP32, providing various string manipulation functions and helpers for formatting and conversions. This lightweight library offers features like integer-to-string conversion, date and time formatting, and simple string operations.

## Features

- **Integer to String Conversion**: Supports multiple integer types (`int8_t`, `uint8_t`, `int16_t`, `int32_t`, `int64_t`).
- **Float and Double to String Conversion**: Customizable decimal precision.
- **Date and Time Formatting**: Includes functions to generate strings for the current date, time, and build timestamp.
- **String Manipulation**:
  - Replace underscores with spaces or vice versa.
  - Convert strings to lowercase.
  - Check if a string is a number.
- **Utility Functions**:
  - Format bytes to human-readable strings (e.g., `Bytes`, `kB`, `MB`, `GB`).
  - Hashing functions for strings.
  - Safe string concatenation.
  - AES encryption and decryption functions

## Installation

### Arduino IDE

1. Download the latest version of the library from the [GitHub Repository](https://github.com/dewenni/EspStrUtil).
2. Extract the ZIP file into your Arduino `libraries` folder.
3. Restart the Arduino IDE.

### PlatformIO

Add the following line to your `platformio.ini`:

```ini
lib_deps =
    https://github.com/dewenni/EspStrUtil
```
