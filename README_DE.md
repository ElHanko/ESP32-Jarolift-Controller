<div align="center">
  <img style="width: 100px;" src="./Doc/ESP32_Jarolift_Controller_Logo.svg">

  <h2>ESP32-Jarolift-Controller</h2>

  ESP32- und CC1101-Steuerung für Jarolift-TDEF-Rollläden auf 433 MHz
</div>

---

**[🇬🇧 English documentation](README.md)**

---

<div align="center">

[![Version](https://img.shields.io/github/v/tag/ElHanko/ESP32-Jarolift-Controller?label=version)](https://github.com/ElHanko/ESP32-Jarolift-Controller/tags)
![GitHub last commit](https://img.shields.io/github/last-commit/ElHanko/ESP32-Jarolift-Controller)
[![License](https://img.shields.io/github/license/ElHanko/ESP32-Jarolift-Controller)](LICENSE)

</div>

---

> [!NOTE]
> Dieses Repository ist ein gepflegter Fork von
> [dewenni/ESP32-Jarolift-Controller](https://github.com/dewenni/ESP32-Jarolift-Controller).
>
> Der Fork behält die eigentliche Jarolift-Steuerung des Originalprojekts bei,
> konzentriert sich aber auf einen reproduzierbaren Build, eine kleinere
> Angriffsfläche und die Hardwarekonfiguration, die in diesem Fork tatsächlich
> verwendet und getestet wird.

Aktuelle Version: **2026.2.2**

Versionsschema:

```text
Jahr.Major.Bugfix
```

Beispiele:

```text
2026.1.0
2026.1.1
2026.2.0
2027.1.0
```

# Überblick

ESP32-Jarolift-Controller steuert Jarolift-TDEF-kompatible
433-MHz-Rollläden mit einem ESP32 und einem CC1101-Funkmodul.

Der ESP32 verhält sich dabei wie ein zusätzlicher Jarolift-Funksender. Er
besitzt eine eigene Sender-Seriennummer sowie einen eigenen
KeeLoq-Rolling-Counter und kann die Rollläden über folgende Schnittstellen
steuern:

- WebUI
- MQTT
- Home Assistant über MQTT Discovery
- Timer
- vordefinierte Gruppen
- direkte Gruppen über Bitmasken

Funksignale vorhandener Jarolift-Fernbedienungen können außerdem empfangen
und über MQTT ausgegeben werden.

# Funktionen

- webbasierte Konfiguration und Steuerung
- MQTT-Steuerung und Statusmeldungen
- Home-Assistant-MQTT-Discovery
- bis zu 16 Rollladenkanäle
- bis zu 6 vordefinierte Gruppen
- beliebige Gruppen über eine 16-Bit-Bitmaske
- Timersteuerung
- Sonnenaufgang und Sonnenuntergang als Trigger
- Empfang vorhandener Jarolift-Fernbedienungen
- lokales Firmware-Update über die WebUI
- Import und Export der Konfiguration
- persistenter KeeLoq-Gerätezähler
- WLAN
- optional W5500-Ethernet

# Unterschiede zum Upstream

Dieser Fork enthält derzeit unter anderem folgende Änderungen gegenüber dem
Originalprojekt:

- reproduzierbarer Docker-basierter Build
- Docker-basiertes Flashen ohne lokale PlatformIO- oder esptool-Installation
- ausgewählte Projektabhängigkeiten direkt im Repository
- lokale Build-Secrets außerhalb von Git
- individueller Schlüssel für die Konfigurationsverschlüsselung statt eines
  gemeinsam verwendeten fest eingebauten Schlüssels
- WPA2-geschützter Access Point im Setup Mode
- verpflichtende WebUI-Authentifizierung im Normalbetrieb
- ArduinoOTA entfernt
- GitHub-basiertes OTA entfernt
- aktive Telnet-Schnittstelle entfernt
- lokales authentifiziertes WebUI-Firmware-Update bleibt erhalten
- Schutz persistenter Daten wie des KeeLoq-Gerätezählers
- korrigierte Initialisierung der CC1101-Sendeleistung
- Validierung der Jarolift-Sender-Seriennummer
- Schutz vor dem Senden mit einer ungültigen Sender-Seriennummer

# Projektstatus

Der aktuell getestete Build dieses Forks ist:

**klassischer ESP32 mit 4 MB Flash**

Entwicklung und Hardwaretests erfolgen derzeit mit einem
ESP32-WROOM-32-basierten DevKit.

Der Quellcode enthält weiterhin vom Upstream übernommene
PlatformIO-Umgebungen für weitere ESP32-Varianten. Diese sind aktuell aber
nicht Bestandteil des getesteten Build- und Release-Wegs dieses Forks.

# Hardware

## ESP32

Empfohlen:

- klassischer ESP32
- ESP32-WROOM-32
- 4 MB Flash

## CC1101 433 MHz

Getestete Hardware:

- EBYTE E07-M1101D-SMA V2.0

Andere kompatible CC1101-Module für 433 MHz können ebenfalls funktionieren.

> [!IMPORTANT]
> Der CC1101 ist ein **3,3-V-Baustein**.
>
> Das Funkmodul darf nicht mit 5 V versorgt werden.

Vor dem Senden sollte eine geeignete 433-MHz-Antenne angeschlossen sein.

## Verkabelung

Standardbelegung dieses Projekts:

| CC1101 | ESP32 |
|---|---:|
| VCC | 3,3 V |
| GND | GND |
| GDO0 | GPIO 21 |
| GDO2 | GPIO 22 |
| SCK | GPIO 18 |
| MOSI | GPIO 23 |
| MISO | GPIO 19 |
| CSN | GPIO 5 |

Die GPIO-Zuordnung kann später in der WebUI geändert werden.

Weitere vom Upstream übernommene Hardwaredokumentation und Bilder befinden
sich im Ordner [`Doc`](Doc/).

## Optionales W5500-Ethernet

Die Unterstützung für einen W5500-Ethernet-Controller wurde vom Upstream
beibehalten.

Da der CC1101 die primäre SPI-Schnittstelle verwendet, nutzt der W5500 eine
separate SPI-Schnittstelle.

Typische Upstream-Konfiguration:

| Signal | ESP32 |
|---|---:|
| CLK | GPIO 25 |
| MOSI | GPIO 26 |
| MISO | GPIO 27 |
| CS | GPIO 32 |
| INT | GPIO 33 |
| RST | GPIO 17 |

Im Setup Mode wird Ethernet nicht gestartet.

# Erste Schritte

## Voraussetzungen

Der unterstützte Build- und Flash-Weg benötigt auf dem Host lediglich:

- Git
- Docker
- USB-Zugriff auf den ESP32 zum Flashen

PlatformIO, Python-Pakete und esptool müssen nicht lokal installiert werden.

Repository klonen:

```sh
git clone https://github.com/ElHanko/ESP32-Jarolift-Controller.git
cd ESP32-Jarolift-Controller
```

# Lokale Secrets

Die Firmware benötigt eine lokale Secrets-Datei, die absichtlich nicht in Git
gespeichert wird.

Vorlage kopieren:

```sh
cp include/local_secrets.example.h include/local_secrets.h
```

Anschließend bearbeiten:

```text
include/local_secrets.h
```

Es werden zwei Werte benötigt.

## Passwort für den Setup Mode

```cpp
#define SETUP_AP_PASSWORD "change-this-password"
```

Das Passwort muss zwischen 8 und 63 Zeichen lang sein.

Es schützt den temporären WLAN-Access-Point des Setup Mode.

## Schlüssel für die Konfigurationsverschlüsselung

Die Datei enthält außerdem einen 16 Byte langen Schlüssel:

```cpp
static constexpr unsigned char CONFIG_ENCRYPTION_KEY[16] = {
  ...
};
```

Vor der ersten Nutzung sollte dafür ein individueller Zufallswert erzeugt
werden.

Der echte Schlüssel darf nicht committed werden.

> [!IMPORTANT]
> `include/local_secrets.h` sollte sicher gesichert werden.
>
> Bei späteren Firmware-Updates sollte weiterhin derselbe
> `CONFIG_ENCRYPTION_KEY` verwendet werden. Wird der Schlüssel geändert,
> können bereits gespeicherte Passwörter unter Umständen nicht mehr
> entschlüsselt werden.

# Build

## Persönlicher Build

Die private Secret-Datei wie oben beschrieben anlegen und individuell
anpassen. Anschließend wird die Firmware gebaut mit:

```sh
./build/build.sh
```

Dieser Modus verwendet ausschließlich `include/local_secrets.h` und bricht
ab, wenn die Datei fehlt. Der komplette Build läuft innerhalb eines
Docker-Containers.

Die erzeugten Dateien liegen anschließend unter:

```text
build/artifacts/
```

Relevante Artefakte:

```text
firmware.bin
firmware_merged.bin
bootloader.bin
partitions.bin
SHA256SUMS
```

`firmware.bin` ist das Applikations-/OTA-Image. `firmware_merged.bin` ist das
vollständige kombinierte Image.

`SHA256SUMS` enthält die Prüfsummen aller erzeugten Binärdateien.

Das unterstützte Build-Skript baut aktuell die PlatformIO-Umgebung:

```text
esp32
```

## Öffentlicher Release-Build

Öffentliche Release-Artefakte werden gebaut mit:

```sh
./build/build.sh release
```

Dieser Modus verwendet die öffentlichen Werte aus
`include/local_secrets.example.h`. Er erzeugt
`include/default_local_secrets.h` ausschließlich im temporären
`/work`-Workspace und verwendet sie dort als `include/local_secrets.h`.

Zusätzlich zu den generischen Artefakten erzeugt der Release-Modus:

```text
esp32_jarolift_ota_<VERSION>.bin
esp32_jarolift_flash_<VERSION>.bin
```

Die versionierte OTA-Datei ist bytegleich mit `firmware.bin`; die
versionierte Flash-Datei ist bytegleich mit `firmware_merged.bin`.

> [!WARNING]
> Vorgefertigte öffentliche Release-Binaries enthalten öffentliche
> Default-Secrets. Deren `SETUP_AP_PASSWORD` und `CONFIG_ENCRYPTION_KEY` sind
> nicht geheim.
>
> Für dauerhafte Installationen wird ausdrücklich ein individueller
> Build mit privater `include/local_secrets.h` empfohlen. Diese Datei niemals
> committen. Ein Wechsel des `CONFIG_ENCRYPTION_KEY` kann bereits gespeicherte
> verschlüsselte Zugangsdaten unlesbar machen.

# Flashen

Auch beim Flashen wird esptool ausschließlich innerhalb eines
Docker-Containers ausgeführt.

Der standardmäßig verwendete serielle Port ist:

```text
/dev/ttyUSB0
```

Ein anderer Port kann als zweites Argument angegeben werden.

## Erstinstallation

```sh
./build/flash.sh install
```

oder:

```sh
./build/flash.sh install /dev/ttyUSB0
```

Dabei wird:

```text
firmware_merged.bin
```

an:

```text
0x0
```

geschrieben.

Vor dem Schreiben verlangt das Skript eine ausdrückliche Bestätigung.

> [!WARNING]
> `install` ist für die Erstinstallation gedacht.
>
> Das vollständige Image überschreibt auch den persistenten NVS-Bereich.
> Vorhandene Konfigurationen und der Jarolift-Rolling-Counter können dadurch
> verloren gehen.

## Firmware-Update

Für normale Firmware-Updates wird verwendet:

```sh
./build/flash.sh update
```

oder:

```sh
./build/flash.sh update /dev/ttyUSB0
```

Dabei wird ausschließlich:

```text
firmware.bin
```

an:

```text
0x10000
```

geschrieben.

NVS und LittleFS bleiben erhalten.

Das Flash-Skript:

- prüft `SHA256SUMS`
- prüft die Kommunikation mit dem ESP32
- prüft, ob 4 MB Flash erkannt werden
- bricht bei einer fehlgeschlagenen Hardwareprüfung ab

Bei einem bereits eingerichteten Controller sollte für normale Updates
`update` verwendet werden.

`install` ist nur für eine bewusst vollständige Neuinstallation gedacht.

# Setup Mode

Der Setup Mode dient zur Erstkonfiguration und Wiederherstellung.

Er kann über den Multiple-Reset-Detector aktiviert werden, indem der ESP32 mehrfach innerhalb des konfigurierten Zeitfensters neu gestartet wird.

Der Setup Mode wird außerdem aktiviert, wenn für den Normalbetrieb notwendige Netzwerk- oder WebUI-Konfiguration fehlt.

Im Setup Mode erstellt der ESP32 folgenden Access Point:

```text
SSID: ESP32-Jarolift
```

Bei den vorgefertigten Release-Binaries lautet das WPA2-Passwort:

```text
change-this-password
```

Dieses Passwort stammt aus den öffentlichen Default-Secrets und ist daher **nicht geheim**.

Nach dem Verbinden mit dem Access Point ist die WebUI erreichbar unter:

```text
http://192.168.4.1
```

Die zusätzliche WebUI-Authentifizierung ist im Setup Mode deaktiviert, da der Zugang bereits durch das separate WPA2-Netz geschützt wird.

Für eine dauerhafte Installation wird empfohlen, die Firmware selbst zu bauen und in:

```text
include/local_secrets.h
```

ein eigenes `SETUP_AP_PASSWORD` sowie einen eigenen `CONFIG_ENCRYPTION_KEY` zu verwenden.

Als Vorlage dient:

```text
include/local_secrets.example.h
```

Die in den vorgefertigten Release-Binaries enthaltenen Default-Secrets sind öffentlich bekannt und sollten nicht als individuelle Sicherheitsmerkmale betrachtet werden.


# WebUI-Authentifizierung

Im Normalbetrieb ist die Authentifizierung verpflichtend.

Vor dem Verlassen des Setup Mode müssen konfiguriert sein:

- Benutzername
- Passwort

Fehlen gültige WebUI-Zugangsdaten, wechselt der Controller wieder in den
Setup Mode, anstatt ungeschützt in den Normalbetrieb zu starten.

> [!IMPORTANT]
> Die WebUI verwendet HTTP.
>
> Der Controller sollte nicht direkt aus dem öffentlichen Internet erreichbar
> sein.

# Konfiguration

In der WebUI können unter anderem folgende Bereiche konfiguriert werden:

- WLAN
- optional W5500-Ethernet
- WebUI-Authentifizierung
- NTP
- MQTT
- Home Assistant
- GPIO
- Jarolift-Protokoll
- Rollläden
- Gruppen
- Timer
- bekannte Fernbedienungen
- Sprache

Änderungen werden automatisch gespeichert.

Einige Hardware- und Jarolift-Einstellungen benötigen einen Neustart, bevor
sie vollständig wirksam werden.

# Jarolift-Konfiguration

## Master Keys

Für das Jarolift-Protokoll werden die entsprechenden KeeLoq-Master-Key-
Einstellungen benötigt.

Die Schlüssel werden nicht in diesem Repository hinterlegt.

## Sender-Seriennummer des Controllers

Der ESP32 arbeitet als eigener Jarolift-Funksender und benötigt daher eine
eigene Sender-Seriennummer.

Die konfigurierte Basis-Seriennummer ist ein **20-Bit-Wert** und wird als
sechsstellige Hexadezimalzahl angegeben:

```text
000001
...
0FFFFF
```

Die erste Hexadezimalstelle muss deshalb immer:

```text
0
```

sein.

Gültige Beispiele:

```text
000010
012345
0abcde
0fffff
```

Ungültige Beispiele:

```text
123456
5c9163
ffffff
```

> [!IMPORTANT]
> Es darf keine sechsstellige Seriennummer verwendet werden, deren erste
> Stelle ungleich `0` ist.
>
> Die vollständige Jarolift-Seriennummer auf Funkebene ist 28 Bit breit.
> Der Controller hängt an die 20-Bit-Basis zusätzlich die Kanalnummer an.
>
> Werte größer als `0x0FFFFF` würden in die Funktionsbits des gesendeten
> Telegramms hineinragen. Dadurch können normale Befehle als andere
> Jarolift-Funktionen übertragen werden.

Die Firmware akzeptiert deshalb für die Controller-Seriennummer ausschließlich:

```text
000001 .. 0FFFFF
```

und verweigert das Senden, wenn eine ungültige Seriennummer in der
Konfiguration vorhanden ist.

## Gerätezähler

Jarolift verwendet KeeLoq-Rolling-Codes.

Der ESP32 führt deshalb einen persistenten Gerätezähler.

Normale Firmware-Updates erhalten diesen Zähler.

Bei einem bereits angelernten Controller sollte der NVS-Bereich nicht
zurückgesetzt oder überschrieben werden, sofern nicht bewusst eine
Neuinitialisierung durchgeführt werden soll.

# Rollläden konfigurieren

Es können bis zu 16 Rollladenkanäle angelegt werden.

Für jeden Kanal können unter anderem festgelegt werden:

- aktiv/inaktiv
- individueller Name
- Jarolift-Kanal

Die konfigurierten Rollläden können anschließend über WebUI und MQTT bedient
werden.

# Rollläden anlernen

Ein Controller-Kanal wird im Prinzip genauso angelernt wie eine zusätzliche
Jarolift-Fernbedienung.

## Über die Anlerntaste am Motor

1. Motor über seine Programmiertaste in den Anlernmodus bringen.
2. Der Motor bestätigt dies durch eine kurze Bewegung bzw. Vibration.
3. Innerhalb des Anlernfensters in der WebUI beim gewünschten Kanal den
   Learn-Button drücken.
4. Der Motor sollte den neuen Sender anschließend bestätigen.

## Durch Kopieren einer vorhandenen Fernbedienung

Mit einer bereits angelernten kompatiblen Jarolift-Fernbedienung:

1. Gewünschten Kanal auswählen.
2. **AUF + AB** gleichzeitig drücken.
3. Auf derselben Fernbedienung **STOP achtmal** drücken.
4. Der Motor bestätigt, dass ein weiterer Sender angelernt werden kann.
5. Innerhalb des Anlernfensters in der WebUI beim gewünschten Kanal den
   Learn-Button drücken.
6. Der Controller sendet die benötigte Lernsequenz.
7. Der Motor sollte den neuen Sender bestätigen.

Vorhandene physische Fernbedienungen bleiben bestehen, wenn der ESP32 als
zusätzlicher Sender angelernt wird.

# Fernbedienungen empfangen

Der Controller kann außerdem Telegramme kompatibler
Jarolift-Fernbedienungen empfangen.

Vorhandene Fernbedienungen können in der WebUI hinterlegt werden, damit
empfangene Befehle bestimmten Rollläden zugeordnet werden können.

Dadurch kann der intern angenommene Rollladenstatus auch aktualisiert werden,
wenn eine physische Fernbedienung verwendet wird.

Der Empfang ist für Automatisierungen hilfreich, sollte aber nicht als
garantierte Zustandsrückmeldung betrachtet werden.

# Gruppen

Es können bis zu sechs vordefinierte Gruppen angelegt werden.

Über MQTT können zusätzlich beliebige Gruppen direkt als 16-Bit-Bitmaske
angesprochen werden.

Das niederwertigste Bit steht für Rollladen 1.

Beispiel:

```text
0000000000010101
```

entspricht:

```text
1, 3, 5
```

Gleichwertige Payloads:

```text
0b0000000000010101
0x15
21
```

# Timer

Der integrierte Timer kann einzelne Rollläden oder Gruppen in vier Modi
steuern:

- feste Zeit
- Astro / Sonnenaufgang oder Sonnenuntergang
- späterer Zeitpunkt
- früherer Zeitpunkt

„Späterer Zeitpunkt“ verwendet den späteren Wert aus Astro-Zeitpunkt und
konfigurierter Vergleichszeit. „Früherer Zeitpunkt“ verwendet entsprechend
den früheren Wert.

# Konfiguration sichern

Die WebUI enthält einen Dateimanager für die Konfiguration.

Die Controller-Konfiguration liegt in:

```text
config.json
```

Sie kann exportiert und später wieder importiert werden.

Konfigurationssicherungen sollten geschützt aufbewahrt werden, da sie
Informationen über die lokale Installation enthalten.

# Firmware-Update über die WebUI

Ein lokales Firmware-Update ist weiterhin über die authentifizierte WebUI
möglich.

Zuerst Firmware bauen:

```sh
./build/build.sh
```

Anschließend in der WebUI folgende Datei hochladen:

```text
build/artifacts/firmware.bin
```

Für diesen Update-Weg wird ausschließlich das Applikations-Image verwendet.

Folgende Update-Mechanismen des Upstreams werden in diesem Fork bewusst nicht
verwendet:

- automatisches GitHub-OTA
- ArduinoOTA
- unauthentifiziertes Remote-Flashing

# MQTT

Das in den Einstellungen konfigurierte Basis-Topic wird im Folgenden
dargestellt als:

```text
<topic>
```

## Rollladenbefehle

### Hoch

```text
topic:   <topic>/cmd/shutter/1 ... <topic>/cmd/shutter/16
payload: UP
         OPEN
         0
```

### Runter

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

### Schattenposition

```text
topic:   <topic>/cmd/shutter/1 ... <topic>/cmd/shutter/16
payload: SHADE
         3
```

## Vordefinierte Gruppen

```text
<topic>/cmd/group/1
...
<topic>/cmd/group/6
```

Unterstützte Befehle:

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

## Gruppen über Bitmaske

```text
<topic>/cmd/group/up
<topic>/cmd/group/down
<topic>/cmd/group/stop
<topic>/cmd/group/shade
```

Der Payload enthält die gewünschte 16-Bit-Bitmaske.

Beispiel für Rollladen 1, 3 und 5:

```text
0b0000000000010101
```

oder:

```text
0x15
```

oder:

```text
21
```

# MQTT-Status

## Rollladenstatus

Der Controller veröffentlicht einen intern abgeleiteten Rollladenstatus.

Typische Werte:

```text
OFFEN      -> 0
GESCHLOSSEN -> 100
SCHATTEN    -> 90
```

> [!IMPORTANT]
> Dies ist keine direkte Positionsrückmeldung des Motors.
>
> Jarolift-TDEF-Motoren liefern über das hier verwendete Funkprotokoll keine
> absolute Rollladenposition zurück. Der Controller leitet seinen Status aus
> den ihm bekannten Befehlen ab.
>
> Eine Bedienung vor Ort, nicht empfangene Funktelegramme oder das Stoppen über
> eine andere Steuerung können deshalb dazu führen, dass der gemeldete Zustand
> nicht mehr der Realität entspricht.

## Empfangene Fernbedienungen

Konfigurierte physische Fernbedienungen können über folgendes Topic
veröffentlicht werden:

```text
<topic>/status/remote/<serial-number>
```

Beispiel:

```json
{
  "name": "<alias-name>",
  "cmd": "<UP, DOWN, STOP, SHADE>",
  "chBin": "<channel-binary>",
  "chDec": "<channel-decimal>"
}
```

# Home Assistant

Die Integration in Home Assistant erfolgt über MQTT Discovery.

Ist sie aktiviert, werden die konfigurierten Rollläden automatisch über den
MQTT-Broker bei Home Assistant angekündigt.

Für den gemeldeten Rollladenstatus gilt dieselbe Einschränkung wie beim
normalen MQTT-Status: Der Zustand wird abgeleitet und ist keine absolute
Positionsrückmeldung des Motors.

# Migration von madmartin/Jarolift_MQTT

Eine Migration von einer vorhandenen Installation von
[madmartin/Jarolift_MQTT](https://github.com/madmartin/Jarolift_MQTT)
ist grundsätzlich möglich.

Wichtige Werte, die dabei erhalten werden müssen:

- GPIO-Konfiguration
- Jarolift-Master-Key-Konfiguration
- Sender-Seriennummer des Controllers
- KeeLoq-Gerätezähler
- Zuordnung der Rollladenkanäle

Sender-Seriennummer und Gerätezähler bestimmen gemeinsam den
Rolling-Code-Zustand des Senders.

Der Gerätezähler eines bereits angelernten Senders sollte nicht willkürlich
zurückgesetzt werden.

Wird bewusst eine neue ESP32-Senderidentität verwendet, sollte dieser neue
Sender stattdessen regulär an den Motoren angelernt werden.

# Änderungen vom Upstream übernehmen

Das Originalprojekt wird bei diesem Fork typischerweise als Git-Remote
`upstream` geführt.

Lokale Struktur:

```text
origin    -> ElHanko/ESP32-Jarolift-Controller
upstream  -> dewenni/ESP32-Jarolift-Controller
```

Änderungen aus dem Upstream sollten vor einer Übernahme geprüft werden, da
dieser Fork sich bewusst in den Bereichen Sicherheit, Build und Update-
Architektur unterscheidet.

# Versionierung

Dieser Fork verwendet:

```text
Jahr.Major.Bugfix
```

Beispiel:

```text
2026.1.0
```

Bedeutung:

```text
2026 = Jahr
1    = größere Veröffentlichung innerhalb des Jahres
0    = Bugfix-Stand
```

Ein Bugfix erhöht die letzte Stelle:

```text
2026.1.1
```

Eine größere funktionale Veröffentlichung erhöht die mittlere Stelle:

```text
2026.2.0
```

# Sicherheitshinweise

Der Controller ist für den Betrieb in einem vertrauenswürdigen lokalen
Netzwerk vorgesehen.

Empfohlen wird:

- individuelles Passwort für den Setup Mode
- individueller Schlüssel für die Konfigurationsverschlüsselung
- starkes WebUI-Passwort
- `include/local_secrets.h` nicht veröffentlichen
- WebUI nicht direkt ins Internet freigeben
- Konfiguration und lokale Secrets sicher sichern
- für normale Firmware-Aktualisierungen `update` statt `install` verwenden

# Credits und Upstream

Dieses Projekt basiert auf:

[dewenni/ESP32-Jarolift-Controller](https://github.com/dewenni/ESP32-Jarolift-Controller)

Das Originalprojekt basiert wiederum auf Ideen und Code von:

[madmartin/Jarolift_MQTT](https://github.com/madmartin/Jarolift_MQTT)

Die ursprüngliche Analyse des Jarolift-Protokolls und frühere
Controller-Arbeiten gehen außerdem auf Arbeiten von Steffen Hille und das
Bastelbudenbuben-Projekt zurück.

Dieser Fork beansprucht keine Urheberschaft für das ursprüngliche Projekt
oder die zugrunde liegende Jarolift-Protokollimplementierung.

# Haftungsausschluss

Dies ist ein unabhängiges Open-Source-Projekt.

Es besteht keine Verbindung zum Hersteller von Jarolift-Produkten und keine
Unterstützung oder Freigabe durch diesen.

Jarolift ist eine Marke des jeweiligen Rechteinhabers.

Für Funksender gelten die jeweils lokalen gesetzlichen Bestimmungen. Der
Benutzer ist selbst dafür verantwortlich, geeignete Hardware innerhalb der
an seinem Standort geltenden Vorschriften zu betreiben.

Die Nutzung erfolgt auf eigene Verantwortung.

# Lizenz

Siehe [`LICENSE`](LICENSE) sowie die jeweiligen Lizenzdateien der eingebundenen
und vendorten Drittanbieter-Komponenten.