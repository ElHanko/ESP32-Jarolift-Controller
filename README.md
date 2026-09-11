<div align="center">
  <img style="width: 100px;" src="./Doc/ESP32_Jarolift_Controller_Logo.svg">

  <h2>ESP32-Jarolift-Controller</h2>

  ESP32 + CC1101 controller for Jarolift TDEF 433 MHz roller shutters
</div>

---

**[🇩🇪 Deutsche Dokumentation](README_DE.md)**

---

<div align="center">

[![Version](https://img.shields.io/github/v/tag/ElHanko/ESP32-Jarolift-Controller?label=version)](https://github.com/ElHanko/ESP32-Jarolift-Controller/tags)
![GitHub last commit](https://img.shields.io/github/last-commit/ElHanko/ESP32-Jarolift-Controller)
[![License](https://img.shields.io/github/license/ElHanko/ESP32-Jarolift-Controller)](LICENSE)

</div>

---

> [!NOTE]
> This repository is a maintained fork of
> [dewenni/ESP32-Jarolift-Controller](https://github.com/dewenni/ESP32-Jarolift-Controller).
>
> The project keeps the original Jarolift controller functionality while
> focusing on a reproducible build, a reduced attack surface and the hardware
> configuration actually used and tested by this fork.

Current version: **2026.2.1**

Version scheme:

```text
year.major.bugfix
```

Examples:

```text
2026.1.0
2026.1.1
2026.2.0
2027.1.0
```

# Overview

ESP32-Jarolift-Controller controls Jarolift TDEF-compatible 433 MHz roller
shutters using an ESP32 and a CC1101 transceiver.

The ESP32 acts as an additional Jarolift radio transmitter. It maintains its
own sender serial number and KeeLoq rolling counter and can control shutters
through:

- WebUI
- MQTT
- Home Assistant via MQTT Discovery
- timers
- predefined groups
- direct bitmask groups

Signals from existing Jarolift remote controls can also be received and
reported through MQTT.

# Features

- Web-based configuration and control
- MQTT control and status messages
- Home Assistant MQTT Discovery
- up to 16 shutter channels
- up to 6 predefined groups
- arbitrary groups through a 16-bit bitmask
- timer control
- sunrise and sunset triggers
- reception of existing Jarolift remote controls
- local firmware update through the WebUI
- configuration import/export
- persistent KeeLoq device counter
- WiFi support
- optional W5500 Ethernet support

# Differences from upstream

This fork currently contains several changes compared with the original
project:

- reproducible Docker-based build workflow
- Docker-based flashing without a host installation of PlatformIO or esptool
- selected project dependencies vendored into the repository
- local build secrets kept outside Git
- individual configuration encryption key instead of a shared hard-coded key
- WPA2-protected Setup Mode access point
- mandatory WebUI authentication during normal operation
- removal of ArduinoOTA
- removal of GitHub-based OTA updates
- removal of the active Telnet interface
- local authenticated WebUI firmware updates retained
- protection of persistent state such as the KeeLoq device counter
- corrected CC1101 transmit-power initialization
- validation of the Jarolift controller sender serial number
- protection against transmitting with an invalid controller sender serial

# Project status

The tested build target of this fork is currently:

**Classic ESP32 with 4 MB flash**

The development and hardware validation is currently done with an
ESP32-WROOM-32 based DevKit.

The source tree still contains PlatformIO environments for other ESP32
variants inherited from upstream, but they are currently not part of the
tested build and release workflow of this fork.

# Hardware

## ESP32

Recommended:

- classic ESP32
- ESP32-WROOM-32
- 4 MB flash

## CC1101 433 MHz

Tested hardware:

- EBYTE E07-M1101D-SMA V2.0

Other compatible 433 MHz CC1101 modules may work as well.

> [!IMPORTANT]
> The CC1101 is a **3.3 V device**.
>
> Do not power the radio module with 5 V.

Attach a suitable 433 MHz antenna before transmitting.

## Wiring

Default wiring used by this project:

| CC1101 | ESP32 |
|---|---:|
| VCC | 3.3 V |
| GND | GND |
| GDO0 | GPIO 21 |
| GDO2 | GPIO 22 |
| SCK | GPIO 18 |
| MOSI | GPIO 23 |
| MISO | GPIO 19 |
| CSN | GPIO 5 |

The GPIO assignment can later be changed in the WebUI.

Existing upstream hardware documentation and images can be found in the
[`Doc`](Doc/) directory.

## Optional W5500 Ethernet

Support for a W5500 Ethernet controller is retained from upstream.

The CC1101 uses the primary SPI interface, therefore the W5500 uses a
separate SPI interface.

Typical upstream configuration:

| Signal | ESP32 |
|---|---:|
| CLK | GPIO 25 |
| MOSI | GPIO 26 |
| MISO | GPIO 27 |
| CS | GPIO 32 |
| INT | GPIO 33 |
| RST | GPIO 17 |

Ethernet is not started while the controller is in Setup Mode.

# Getting started

## Requirements

The supported build and flash workflow requires:

- Git
- Docker
- USB access to the ESP32 when flashing

PlatformIO, Python packages and esptool do not have to be installed on the
host system.

Clone the repository:

```sh
git clone https://github.com/ElHanko/ESP32-Jarolift-Controller.git
cd ESP32-Jarolift-Controller
```

# Local secrets

The firmware requires a local secrets file which is intentionally excluded
from Git.

Create it from the example:

```sh
cp include/local_secrets.example.h include/local_secrets.h
```

Then edit:

```text
include/local_secrets.h
```

Two values are required.

## Setup Mode password

```cpp
#define SETUP_AP_PASSWORD "change-this-password"
```

The password must contain between 8 and 63 characters.

It protects the temporary WiFi access point created in Setup Mode.

## Configuration encryption key

The file also contains a 16-byte key:

```cpp
static constexpr unsigned char CONFIG_ENCRYPTION_KEY[16] = {
  ...
};
```

Generate an individual random value before using the controller.

Do not commit the real key.

> [!IMPORTANT]
> Keep a secure backup of `include/local_secrets.h`.
>
> Firmware updates should continue to use the same
> `CONFIG_ENCRYPTION_KEY`. Changing this key can make passwords already stored
> in the configuration impossible to decrypt.

# Build

Build the firmware with:

```sh
./build/build.sh
```

The build runs inside Docker.

The resulting files are written to:

```text
build/artifacts/
```

Relevant artifacts:

```text
firmware.bin
firmware_merged.bin
esp32_jarolift_ota_2026.2.1.bin
esp32_jarolift_flash_2026.2.1.bin
bootloader.bin
partitions.bin
SHA256SUMS
```

`firmware.bin` and `esp32_jarolift_ota_2026.2.1.bin` are the
application/OTA image. `firmware_merged.bin` and
`esp32_jarolift_flash_2026.2.1.bin` are the complete flash image.

`SHA256SUMS` contains checksums for all generated binary files.

The supported build script currently builds the `esp32` PlatformIO
environment.

# Flashing

The flash workflow also runs esptool inside Docker.

The default serial device is:

```text
/dev/ttyUSB0
```

A different port can be supplied as the second argument.

## First installation

```sh
./build/flash.sh install
```

or:

```sh
./build/flash.sh install /dev/ttyUSB0
```

This writes:

```text
firmware_merged.bin
```

at:

```text
0x0
```

The script requires explicit confirmation before the write.

> [!WARNING]
> `install` is intended for the first installation.
>
> The complete image overwrites the persistent NVS area. Existing
> configuration and the Jarolift rolling counter may therefore be lost.

## Firmware update

For normal firmware updates use:

```sh
./build/flash.sh update
```

or:

```sh
./build/flash.sh update /dev/ttyUSB0
```

This writes only:

```text
firmware.bin
```

at:

```text
0x10000
```

NVS and LittleFS are preserved.

The flash script also:

- verifies `SHA256SUMS`
- checks communication with the ESP32
- verifies that a 4 MB flash chip is detected
- aborts if the hardware check fails

For an already configured controller, use `update` unless a complete
reinstallation is explicitly required.

# Setup Mode

Setup Mode is used for initial configuration or recovery.

It can be triggered through the multiple-reset detector by restarting the
ESP32 repeatedly within the configured time window.

Setup Mode is also entered when required network or WebUI configuration is
missing.

While Setup Mode is active, the controller creates:

```text
SSID: ESP32-Jarolift
```

The WPA2 password is defined by:

```text
SETUP_AP_PASSWORD
```

in:

```text
include/local_secrets.h
```

After connecting to the access point, open:

```text
http://192.168.4.1
```

WebUI authentication is disabled in Setup Mode because access is protected
by the dedicated WPA2 network.

# WebUI authentication

Authentication is mandatory during normal operation.

Configure:

- username
- password

before leaving Setup Mode.

If valid WebUI credentials are missing, the controller returns to Setup Mode
instead of starting normal operation without authentication.

> [!IMPORTANT]
> The WebUI uses HTTP.
>
> Do not expose the controller directly to the public Internet.

# Configuration

The WebUI contains settings for:

- WiFi
- optional W5500 Ethernet
- WebUI authentication
- NTP
- MQTT
- Home Assistant
- GPIO
- Jarolift protocol
- shutters
- groups
- timers
- known remote controls
- language

Changes are saved automatically.

Some hardware and Jarolift settings require a restart before they take full
effect.

# Jarolift configuration

## Master keys

The Jarolift protocol settings require the appropriate KeeLoq master-key
configuration.

The keys are not stored in this repository.

## Controller sender serial

The ESP32 acts as its own Jarolift transmitter and therefore needs its own
sender serial prefix.

The configured base serial is a **20-bit value** and must be represented by
six hexadecimal digits:

```text
000001
...
0FFFFF
```

The first hexadecimal digit must therefore always be:

```text
0
```

Examples:

```text
000010
012345
0abcde
0fffff
```

Invalid:

```text
123456
5c9163
ffffff
```

> [!IMPORTANT]
> Do not use a six-digit value whose first digit is non-zero.
>
> The complete on-air Jarolift serial is 28 bits. The controller appends the
> channel number to the configured 20-bit base serial.
>
> Values larger than `0x0FFFFF` would overlap with the function bits in the
> transmitted telegram and can turn normal commands into different Jarolift
> functions.

The firmware therefore rejects sender serials outside:

```text
000001 .. 0FFFFF
```

and refuses to transmit if an invalid value is present in the configuration.

## Device counter

Jarolift uses KeeLoq rolling codes.

The ESP32 therefore maintains a persistent device counter.

Normal firmware updates preserve this counter.

Avoid resetting or overwriting NVS on an already learned controller unless
you deliberately intend to reinitialize it.

# Configuring shutters

Up to 16 shutter channels can be configured.

Each channel can have:

- enabled/disabled state
- individual name
- Jarolift channel assignment

The configured shutters can then be controlled through the WebUI and MQTT.

# Teach-in of shutters

A controller channel can be taught to a motor in the same general way as an
additional Jarolift remote control.

## Using the motor learn button

1. Put the motor into learn mode using its programming button.
2. The motor confirms this with a short movement/vibration.
3. Within the learning window, press the corresponding Learn button in the
   controller WebUI.
4. The motor should confirm the new transmitter.

## Copying an existing remote

With an already learned compatible Jarolift remote:

1. Select the required channel.
2. Press **UP + DOWN** simultaneously.
3. Press **STOP eight times** on the existing remote.
4. The motor confirms that it is ready to learn another transmitter.
5. Within the learning window, press the corresponding Learn button in the
   controller WebUI.
6. The controller transmits the required learn sequence.
7. The motor should confirm the new transmitter.

Existing physical remotes remain paired when an additional ESP32 sender is
learned.

# Remote controls

The controller can also receive compatible Jarolift remote-control
telegrams.

Existing remotes can be configured in the WebUI so that received commands
can be associated with shutters.

This makes it possible to update the internally inferred shutter state when
a physical remote is used.

Reception is useful for automation but should not be treated as guaranteed
state feedback.

# Groups

Up to six predefined shutter groups can be configured.

MQTT also supports arbitrary groups using a 16-bit bitmask.

The least significant bit represents shutter 1.

Example:

```text
0000000000010101
```

selects:

```text
1, 3, 5
```

Equivalent payload representations:

```text
0b0000000000010101
0x15
21
```

# Timers

The integrated timer can control individual shutters or groups using four
modes:

- fixed time
- Astro / sunrise or sunset
- later time
- earlier time

Later time uses the later of the astronomical event and the configured
comparison time. Earlier time uses the earlier of those two times.

# Configuration backup

The WebUI includes a configuration file manager.

The controller configuration is stored in:

```text
config.json
```

It can be exported and later imported again.

Keep configuration backups secure because they contain information about
your local controller setup.

# Firmware update through the WebUI

A local firmware update remains available through the authenticated WebUI.

Build the firmware first:

```sh
./build/build.sh
```

Then upload:

```text
build/artifacts/firmware.bin
```

through the firmware update page in the WebUI.

Only the application image should be used for this type of update.

The following upstream update methods are intentionally not used by this
fork:

- GitHub automatic OTA
- ArduinoOTA
- unauthenticated remote flashing

# MQTT

The configured base MQTT topic is represented below as:

```text
<topic>
```

## Shutter commands

### Up

```text
topic:   <topic>/cmd/shutter/1 ... <topic>/cmd/shutter/16
payload: UP
         OPEN
         0
```

### Down

```text
topic:   <topic>/cmd/shutter/1 ... <topic>/cmd/shutter/16
payload: DOWN
         CLOSE
         1
```

### Stop

```text
topic:   <topic>/cmd/shutter/1 ... <topic>/cmd/shutter/16
payload: STOP
         2
```

### Shade

```text
topic:   <topic>/cmd/shutter/1 ... <topic>/cmd/shutter/16
payload: SHADE
         3
```

## Predefined group commands

```text
<topic>/cmd/group/1
...
<topic>/cmd/group/6
```

Supported commands:

```text
UP
OPEN
0

DOWN
CLOSE
1

STOP
2

SHADE
3
```

## Bitmask group commands

```text
<topic>/cmd/group/up
<topic>/cmd/group/down
<topic>/cmd/group/stop
<topic>/cmd/group/shade
```

The payload is the desired 16-bit shutter mask.

Example for shutters 1, 3 and 5:

```text
0b0000000000010101
```

or:

```text
0x15
```

or:

```text
21
```

# MQTT status

## Shutter state

The controller publishes an internally inferred shutter state.

Typical values:

```text
OPEN   -> 0
CLOSED -> 100
SHADE  -> 90
```

> [!IMPORTANT]
> This is not direct position feedback from the motor.
>
> Jarolift TDEF motors do not provide an absolute shutter position through
> the radio protocol used here. The controller derives the state from commands
> it knows about.
>
> Physical operation, missed radio telegrams or stopping the shutter from
> another source can therefore make the reported state differ from reality.

## Received remote signals

Configured physical remotes can be published through:

```text
<topic>/status/remote/<serial-number>
```

Example structure:

```json
{
  "name": "<alias-name>",
  "cmd": "<UP, DOWN, STOP, SHADE>",
  "chBin": "<channel-binary>",
  "chDec": "<channel-decimal>"
}
```

# Home Assistant

Home Assistant integration is available through MQTT Discovery.

When enabled, configured shutters are announced automatically to Home
Assistant through the configured MQTT broker.

The reported shutter state has the same limitation as the normal MQTT status:
it is inferred and is not absolute position feedback from the motor.

# Migration from madmartin/Jarolift_MQTT

Migration from an existing
[madmartin/Jarolift_MQTT](https://github.com/madmartin/Jarolift_MQTT)
installation is possible.

Important values to preserve are:

- GPIO configuration
- Jarolift master-key configuration
- controller sender serial
- KeeLoq device counter
- shutter/channel assignment

The sender serial and device counter together define the rolling-code state
of the transmitter.

Do not arbitrarily reset the counter of an already learned sender.

If you intentionally choose a new ESP32 sender identity, learn that new
transmitter into the motors instead.

# Updating from upstream

The original project is configured as the Git upstream of this fork.

Typical local repository layout:

```text
origin    -> ElHanko/ESP32-Jarolift-Controller
upstream  -> dewenni/ESP32-Jarolift-Controller
```

Upstream changes should be reviewed before being integrated because this fork
intentionally differs in security, build and update architecture.

# Versioning

This fork uses:

```text
year.major.bugfix
```

Example:

```text
2026.1.0
```

Meaning:

```text
2026 = year
1    = major release within that year
0    = bugfix level
```

A bugfix release increments the last component:

```text
2026.1.1
```

A larger feature release increments the middle component:

```text
2026.2.0
```

# Security notes

This controller operates inside a trusted local network.

Recommended practice:

- use an individual Setup Mode password
- use a unique configuration encryption key
- use a strong WebUI password
- keep `include/local_secrets.h` private
- do not expose the WebUI directly to the Internet
- back up configuration and local secrets securely
- use `update`, not `install`, for normal firmware upgrades

# Credits and upstream

This project is derived from:

[dewenni/ESP32-Jarolift-Controller](https://github.com/dewenni/ESP32-Jarolift-Controller)

which itself builds on ideas and code from:

[madmartin/Jarolift_MQTT](https://github.com/madmartin/Jarolift_MQTT)

The original Jarolift protocol analysis and controller work also traces back
to work by Steffen Hille and the Bastelbudenbuben project.

This fork does not claim authorship of the original project or the underlying
Jarolift protocol implementation.

# Disclaimer

This is an independent open-source project.

It is not affiliated with, endorsed by or supported by the manufacturer of
Jarolift products.

Jarolift is a trademark of its respective owner.

Radio transmitters are subject to local regulations. The user is responsible
for operating compatible hardware within the legal limits applicable at their
location.

Use this software at your own risk.

# License

See [`LICENSE`](LICENSE) and the individual license files of vendored and
third-party components.