// --------------------------------------
// localization texts
// --------------------------------------
const user_translations = {
  timer: {
    de: "Timer",
    en: "Timer",
  },
  led_setup: {
    de: "LED-Setup-Mode",
    en: "LED-Setup-Mode",
  },
  learn_mode: {
    de: "neuer Anlernmodus",
    en: "new learn mode",
  },
  shutter: {
    de: "Rolladen",
    en: "Shutter",
  },
  serial_nr: {
    de: "Seriennummer",
    en: "serial number",
  },
  set_shade: {
    de: "setze Schatten",
    en: "set shade",
  },
  learn: {
    de: "anlernen",
    en: "learn",
  },
  channel: {
    de: "Kanal",
    en: "Channel",
  },
  info_leanmode: {
    de: "Der neue Lernmodus ist für Empfänger neuer als 2010, die eine Lernsequenz haben, bei der die Tasten UP+DOWN gleichzeitig gedrückt werden und dann die Taste STOP. Die alte Lernmethode für Empfänger, die vor 2010 hergestellt wurden, verwendet eine spezielle LEARN-Taste.",
    en: "new learn mode is for receivers newer than 2010, which have a learning sequence of buttons UP+DOWN pressed simultaneously, then press STOP. the old learn method for receivers manufactured before 2010 uses a special LEARN button.",
  },
  info_serial: {
    de: "Jeder Kanal benötigt eine individuelle Seriennummer. Wenn Sie hier ein neues Seriennummernpräfix eingeben, ändern sich die Seriennummern für alle Kanäle. ACHTUNG: alle Empfänger müssen danach (erneut) eingelernt werden und der Device Counter muss zurückgesetzt werden!",
    en: "Each channel requires an individual serial number. If you enter a new serial number prefix here, the serial numbers for all channels will change. ATTENTION: all receivers must then be learned (again) and the device counter must be reset!",
  },
  info_devcnt: {
    de: "Der Zähler wird zusammen mit jedem Datagramm über den Funk gesendet. Er beginnt bei null und wird bei jedem Datagramm inkrementiert. Jeder empfänger hört die empfangenen Datagramme ab und zeichnet den Zähler des Senders auf. Wenn der Zähler des Senders und des Empfängers zu sehr voneinander abweichen, müssen Sie den Empfänger neu anlernen. Um dies zu vermeiden, wenn Sie einen Dongle austauschen (oder wenn ein Update schief geht), können Sie hier einen Zähler ungleich null eingeben. ACHTUNG: lassen Sie die Finger davon, wenn Sie die Wirkung nicht verstehen, Sie riskieren, dass Sie alle Ihre Empfänger (wieder) neu anlernen müssen!",
    en: "the device counter is send together with every datagram over the radio. beginning with zero, it is incremented on every datagram. each receiver listens to received datagrams and records the sender's device counter. when the sender's and receiver's device counter differ too much, you must re-learn the receiver. to avoid this when replacing a dongle (or when an update goes wrong) you can enter a non-zero device counter here. ATTENTION: don't touch this if you don't understand the effect, you risk to re-learn all your receivers (again)!",
  },
  help: {
    de: "Hilfe",
    en: "Help",
  },
  gpio_restart_info: {
    de: "Änderungen an den GPIO Einstellungen benötigen einen Neustart!",
    en: "Changes to GPIO settings require a restart!",
  },
  gpio_info: {
    de: "Beispiel für einen typischen ESP32",
    en: "example for typical ESP32",
  },
  groups: {
    de: "Gruppen",
    en: "Groups",
  },
  group: {
    de: "Gruppe",
    en: "Group",
  },
  channels: {
    de: "Kanäle",
    en: "Channels",
  },
  name: {
    de: "Name",
    en: "Name",
  },
  mask: {
    de: "Bitmaske",
    en: "Bitmask",
  },
  mask_help: {
    de: "In diesem Feld wird über eine Bitmaske festgelegt, welche Kanäle zu dieser Gruppe gehören.\nDie Bitmaske ist eine 16-Bit-Zahl, wobei das niederwertigste Bit (rechts) den Kanal 1 repräsentiert. \nEin gesetztes Bit bedeutet, dass der Kanal zu dieser Gruppe gehört.\n\nBeispiel: 0000000000010101 bedeutet, dass die Kanäle 1, 3 und 5 zu dieser Gruppe gehören.",
    en: "In this field, a bitmask is used to determine which channels belong to this group.\nThe bitmask is a 16-bit number, with the least significant bit (right) representing channel 1.\nA set bit means that the channel belongs to this group.\n\nExample: 0000000000010101 means that channels 1, 3 and 5 belong to this group.",
  },
  grp_mask_help: {
    de: "Sie können auch ein generisches Gruppenkommando verwenden und die Bitmaske verwenden, um die Rolläden direkt auszuwählen. Die Bitmaske ist eine 16-Bit-Zahl, wobei das niederwertigste Bit (rechts) den Kanal 1 repräsentiert. Ein gesetztes Bit bedeutet, dass der Kanal zu dieser Gruppe gehört.\n\nBeispiel: 0000000000010101 bedeutet, dass die Kanäle 1, 3 und 5 zu dieser Gruppe gehören.\n\nAls Payload können Sie drei verschiedene Formate verwenden, um die gleiche Bitmaske darzustellen:",
    en: "You can also use a generic group command and provide the bitmask to select the shutters directly. The bitmask is a 16-bit number, with the least significant bit (on the right) representing channel 1. A set bit means that the channel belongs to this group.\n\nExample: `0000000000010101` means that channels 1, 3, and 5 belong to this group.\n\nAs payload, you can use three different formats to represent the same bitmask:",
  },
  bitmask_wizard: {
    de: "Auswahl Assistent",
    en: "Selection Wizard",
  },
  sunrise: {
    de: "Sonnenaufgang",
    en: "sunrise",
  },
  sundown: {
    de: "Sonnenuntergang",
    en: "sundown",
  },
  cmd_up: {
    de: "Hochfahren",
    en: "up",
  },
  cmd_down: {
    de: "Runterfahren",
    en: "down",
  },
  cmd_shade: {
    de: "Schatten",
    en: "shade",
  },
  timer_type: {
    de: "Zeitgeber",
    en: "Time-Source",
  },
  time: {
    de: "Uhrzeit",
    en: "Time",
  },
  time_HH_MM: {
    de: "Zeit (HH:MM)",
    en: "Time (HH:MM)",
  },
  use_min_time: {
    de: "Frühestens um (HH:MM)",
    en: "Not earlier than (HH:MM)",
  },
  use_max_time: {
    de: "Spätestes um (HH:MM)",
    en: "Not later than (HH:MM)",
  },
  offset_desc: {
    de: "Offset in Minuten (z. B. -15 oder +20)",
    en: "Offset in Minutens (e.g. -15 oder +20)",
  },
  command: {
    de: "Befehl",
    en: "Command",
  },
  weekdays: {
    de: "Wochentage",
    en: "weekdays",
  },
  day_mo: {
    de: "Mo",
    en: "Mo",
  },
  day_tu: {
    de: "Di",
    en: "Tu",
  },
  day_we: {
    de: "Mi",
    en: "We",
  },
  day_th: {
    de: "Do",
    en: "Th",
  },
  day_fr: {
    de: "Fr",
    en: "Fr",
  },
  day_sa: {
    de: "Sa",
    en: "Sa",
  },
  day_su: {
    de: "So",
    en: "Su",
  },
  geo_location: {
    de: "Geografische Lage",
    en: "Geographical location",
  },
  geo_info: {
    de: "Wird benötigt für die Timer Funktion mit Sonnenaufgang und Sonnenuntergang",
    en: "Required for the timer function with sunrise and sunset",
  },
  latitude: {
    de: "Breitengrad",
    en: "Latitude",
  },
  longitude: {
    de: "Längengrad",
    en: "Longitude",
  },
  time_info: {
    de: "Zeitinformationen",
    en: "Time information",
  },
  apply: {
    de: "Übernehmen",
    en: "apply",
  },
  cancel: {
    de: "Abbrechen",
    en: "cancel",
  },
  gpio: {
    de: "GPIO-Zuweisung",
    en: "GPIO-Settings",
  },
  remote: {
    de: "Fernbedienung",
    en: "Remote",
  },
  remotes: {
    de: "Fernbedienungen",
    en: "Remotes",
  },
  service: {
    de: "Service",
    en: "Service",
  },
  service_cmds: {
    de: "Service-Befehle",
    en: "Service-Commands",
  },
  cmd_rotation: {
    de: "Drehrichtung ändern",
    en: "Change rotation",
  },
  cmd_endpoint_up: {
    de: "Endlage oben",
    en: "Endpoint up",
  },
  cmd_endpoint_down: {
    de: "Endlage unten",
    en: "Endpoint down",
  },
  set: {
    de: "setzen",
    en: "set",
  },
  delete: {
    de: "löschen",
    en: "delete",
  },
  unlearn: {
    de: "ablernen",
    en: "unlearn",
  },
  help_channels: {
    de: "Unter Kanäle werden die bis zu 16 aktivierten Rollläden mit ihren konfigurierten Namen angezeigt. Jeder Kanal kann mit UP, DOWN, STOP und SHADE gesteuert werden. Name und Sichtbarkeit werden unter Einstellungen > Rollladen festgelegt.",
    en: "The Channels page shows up to 16 enabled shutters with their configured names. Each channel can be controlled with UP, DOWN, STOP and SHADE. Its name and visibility are configured under Settings > Shutter.",
  },
  help_groups: {
    de: "Bis zu sechs aktivierte Gruppen stehen mit denselben vier Befehlen zur Verfügung. Die Bitmaske einer Gruppe wählt die zugehörigen Kanäle aus; das niederwertigste Bit rechts steht für Kanal 1.",
    en: "Up to six enabled groups provide the same four commands. A group's bitmask selects its channels; the least significant bit on the right represents channel 1.",
  },
  help_learn: {
    de: "Unter Einstellungen > Rollladen kann jeder Kanal einzeln angelernt oder abgelernt werden. Der Anlernmodus in den JAROLIFT-Einstellungen wählt zwischen dem neuen und dem alten Anlernverfahren.",
    en: "Under Settings > Shutter, each channel can be learned or unlearned individually. The learn mode in the JAROLIFT settings selects the new or legacy learning procedure.",
  },
  help_service: {
    de: "Die Service-Seite bietet für den ausgewählten aktivierten Kanal UP, DOWN, STOP und SHADE sowie Set Shade und das Setzen oder Löschen der oberen und unteren Endlage.",
    en: "For the selected enabled channel, the Service page provides UP, DOWN, STOP and SHADE, plus Set Shade and setting or deleting the upper and lower end points.",
  },
  help_schedules: {
    de: "Es gibt sechs einzeln aktivierbare Zeitprogramme. Jedes besitzt Wochentage, einen Befehl (UP, DOWN oder SHADE) und eine Bitmaske zur Auswahl der Kanäle.",
    en: "There are six schedules that can be enabled individually. Each has weekdays, a command (UP, DOWN or SHADE), and a bitmask selecting the channels.",
  },
  help_schedule_fixed: {
    de: "Zeitpunkt: Ausführung zu einer festen Uhrzeit.",
    en: "Fixed time: runs at a configured time.",
  },
  help_schedule_astro: {
    de: "Astro: Ausführung bei Sonnenaufgang oder Sonnenuntergang mit einem Offset in Minuten. Dafür werden NTP-Zeit und die geografische Lage benötigt.",
    en: "Astro: runs at sunrise or sunset with an offset in minutes. This requires NTP time and the geographical location.",
  },
  help_schedule_later: {
    de: "Späterer Zeitpunkt: verwendet den späteren Wert aus Astro-Zeit und Vergleichszeit.",
    en: "Later time: uses the later value of the Astro time and comparison time.",
  },
  help_schedule_earlier: {
    de: "Früherer Zeitpunkt: verwendet den früheren Wert aus Astro-Zeit und Vergleichszeit.",
    en: "Earlier time: uses the earlier value of the Astro time and comparison time.",
  },
  help_schedule_legacy: {
    de: "Bestehende Konfigurationen mit gleichzeitig gesetzter frühester und spätester Zeit bleiben unverändert aktiv. Die WebUI zeigt dafür einen Hinweis, bis ein neuer Modus gewählt wird.",
    en: "Existing configurations with both earliest and latest time enabled remain active unchanged. The WebUI shows a notice until a new mode is selected.",
  },
  help_mqtt_intro: {
    de: "<base> steht für das unter Einstellungen konfigurierte MQTT-Basistopic. Topic-Namen sind nicht von der Sprache der WebUI abhängig.",
    en: "<base> represents the MQTT base topic configured under Settings. Topic names do not depend on the WebUI language.",
  },
  help_mqtt_commands: {
    de: "Befehle",
    en: "Commands",
  },
  help_direct_mask: {
    de: "Direkte Bitmaske",
    en: "Direct bitmask",
  },
  help_any_payload: {
    de: "beliebig",
    en: "any",
  },
  help_reconfigure: {
    de: "Home Assistant neu konfigurieren",
    en: "Reconfigure Home Assistant",
  },
  help_mqtt_status: {
    de: "Status",
    en: "Status",
  },
  help_availability: {
    de: "Verfügbarkeit",
    en: "Availability",
  },
  help_diagnostics: {
    de: "Netzwerk/System",
    en: "Network/system",
  },
  help_errors: {
    de: "Fehlermeldungen",
    en: "Error messages",
  },
  help_error_text: {
    de: "Text",
    en: "Text",
  },
  help_mqtt_states: {
    de: "Die Rollladen-Statuswerte sind die intern erwarteten Positionen: 0 für offen, 90 für Schatten und 100 für geschlossen. Die Aktivierungszustände aller sechs Zeitprogramme werden beim MQTT-Verbindungsaufbau und nach einer Änderung als retained true oder false veröffentlicht.",
    en: "Shutter status values are the internally expected positions: 0 for open, 90 for shade and 100 for closed. The enabled state of all six schedules is published as retained true or false when MQTT connects and after a change.",
  },
  help_home_assistant: {
    de: "Wenn Home Assistant aktiviert ist, werden Discovery-Einträge für aktivierte Kanäle und Gruppen, Neustart/Neukonfiguration sowie Netzwerk- und Systemsensoren veröffentlicht. Die Zeitprogramm-Aktivierung steht über die oben genannten MQTT-Topics zur Verfügung.",
    en: "When Home Assistant is enabled, discovery entries are published for enabled channels and groups, restart/reconfigure actions, and network and system sensors. Schedule enable control is available through the MQTT topics shown above.",
  },
  help_remotes: {
    de: "Empfangene Originalfernbedienungen können unter Einstellungen > Fernbedienungen mit Name, sechsstelliger Seriennummer und Kanal-Bitmaske zugeordnet und einzeln aktiviert werden. Bei passenden aktivierten Einträgen aktualisieren UP, DOWN und SHADE auch die erwarteten Kanalpositionen.",
    en: "Received original remotes can be assigned under Settings > Remotes using a name, six-digit serial number and channel bitmask, and enabled individually. For matching enabled entries, UP, DOWN and SHADE also update the expected channel positions.",
  },
  help_remote_mqtt: {
    de: "Jedes empfangene Signal wird unter <base>/status/remote/<serial> als JSON mit name, cmd, chBin und chDec veröffentlicht. chBin enthält die 16-Bit-Kanalauswahl in Binärform, chDec denselben Wert dezimal; unbekannte Fernbedienungen erhalten den Namen unknown.",
    en: "Each received signal is published at <base>/status/remote/<serial> as JSON containing name, cmd, chBin and chDec. chBin contains the 16-bit channel selection in binary form and chDec the same value in decimal; unknown remotes use the name unknown.",
  },
  help_network: {
    de: "Netzwerk und Zugriff",
    en: "Network and access",
  },
  help_network_settings: {
    de: "WLAN ist in einer neuen Konfiguration aktiviert und kann deaktiviert werden, wenn Ethernet verwendet wird. WLAN und W5500-Ethernet unterstützen DHCP oder eine statische IP-Konfiguration. Netzwerk- und GPIO-Änderungen werden nach einem Neustart wirksam.",
    en: "WiFi is enabled in a new configuration and can be disabled when Ethernet is used. WiFi and W5500 Ethernet support DHCP or static IP configuration. Network and GPIO changes take effect after a restart.",
  },
  help_setup_mode: {
    de: "Der Setup-Modus startet den Access Point ESP32-Jarolift unter 192.168.4.1. Er wird bei fehlender oder ungültiger Grundkonfiguration, ohne aktiviertes Netzwerk, ohne Zugangsdaten oder nach fünf schnellen Neustarts im 5-Sekunden-Fenster aktiviert.",
    en: "Setup mode starts the ESP32-Jarolift access point at 192.168.4.1. It is activated for missing or invalid basic configuration, with no network enabled, without credentials, or after five quick restarts within the 5-second window.",
  },
  help_authentication: {
    de: "Im normalen Betrieb schützen Benutzername und Passwort die WebUI, WebSocket-, Config- und OTA-Zugriffe. Es gibt keinen separaten Aktivierungsschalter; im Setup-Modus ist die Anmeldung deaktiviert.",
    en: "In normal operation, username and password protect WebUI, WebSocket, configuration and OTA access. There is no separate enable switch; authentication is disabled in setup mode.",
  },
  help_config: {
    de: "Unter Tools kann config.json angezeigt, heruntergeladen oder hochgeladen werden. Nach einem erfolgreichen Import wird die Konfiguration neu geladen; ein Neustart wendet alle abhängigen Einstellungen vollständig an.",
    en: "Under Tools, config.json can be viewed, downloaded or uploaded. After a successful import the configuration is reloaded; a restart fully applies all dependent settings.",
  },
  help_config_upload: {
    de: "Beim Upload ersetzt nur eine vollständige, syntaktisch gültige JSON-Datei die aktive Konfiguration. Ungültige, abgebrochene oder gleichzeitig gestartete Uploads werden abgelehnt, ohne die aktive Datei zu ersetzen.",
    en: "During upload, only a complete, syntactically valid JSON file replaces the active configuration. Invalid, interrupted or concurrent uploads are rejected without replacing the active file.",
  },
  help_ota: {
    de: "Unter Tools kann ein Firmware-Update manuell als .bin-Datei hochgeladen werden. Verwenden Sie das Anwendungs-/OTA-Abbild (esp32_jarolift_ota_<version>.bin, bei einem lokalen Build firmware.bin), nicht das zusammengeführte Flash-Abbild. Nach erfolgreichem Upload bestätigt die WebUI den Neustart.",
    en: "Under Tools, a firmware update can be uploaded manually as a .bin file. Use the application/OTA image (esp32_jarolift_ota_<version>.bin, or firmware.bin from a local build), not the merged flash image. After a successful upload, the WebUI asks for restart confirmation.",
  },
  help_version_dialog: {
    de: "Ein Klick auf die Versionsnummer zeigt nur die aktuell installierte Version an. Firmware-Updates werden manuell unter Tools gestartet.",
    en: "Clicking the version number only shows the currently installed version. Firmware updates are started manually under Tools.",
  },
  help_system_logging: {
    de: "Systeminformationen und Protokolle",
    en: "System information and logs",
  },
  help_system: {
    de: "Die System-Seite zeigt Netzwerk- und MQTT-Status, Software- und Buildinformationen, Laufzeit, Neustartgrund, Speicherbelegung, Chipdaten sowie aktuelle Zeit, Sonnenaufgang und Sonnenuntergang.",
    en: "The System page shows network and MQTT status, software and build information, uptime, restart reason, memory usage, chip data, and the current time, sunrise and sunset.",
  },
  help_logging: {
    de: "Die Protokollierung kann aktiviert oder deaktiviert werden. Die Protokolle können nach Richtung sortiert, aktualisiert und geleert werden. Verfügbar sind die Stufen Fehler, Warnung, Info und Debug.",
    en: "Logging can be enabled or disabled. Logs can be sorted, refreshed and cleared. The available levels are Error, Warning, Info and Debug.",
  },
};
