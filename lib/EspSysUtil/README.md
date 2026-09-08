# EspSysUtil

EspSysUtil is a utility library for the ESP32 that provides several useful features and classes:

- Managing restart reasons (Restart Reason)
- Watchdog Timer (WDT)
- OTA state management (Over-The-Air Updates)
- Multi-Reset-Detection (MRD)

## Features

- **Restart Reason**: Store and retrieve reasons for device restarts.
- **Watchdog Timer**: Simple access to the ESP32 Task Watchdog Timer.
- **OTA**: Manage the OTA update state.
- **Multi-Reset-Detection (MRD)**: Detect multiple consecutive resets within a configurable timeout.

## Installation

### Arduino IDE

1. Download the latest version of this library from [GitHub](https://github.com/dewenni/EspSysUtil).
2. Extract the ZIP file into your Arduino `libraries` folder.
3. Restart the Arduino IDE.

### PlatformIO

1. Add the library to your `platformio.ini` file:

   ```ini
   lib_deps =
       https://github.com/dewenni/EspSysUtil
