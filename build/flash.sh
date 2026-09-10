#!/bin/bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/build/artifacts"
MODE="${1:-}"
PORT="${2:-/dev/ttyUSB0}"

case "$MODE" in
    install)
        IMAGE="$OUT/firmware_merged.bin"
        OFFSET="0x0"
        ;;
    update)
        IMAGE="$OUT/firmware.bin"
        OFFSET="0x10000"
        ;;
    *)
        echo "Verwendung:"
        echo "  $0 install [port]   # Erstinstallation, löscht NVS"
        echo "  $0 update  [port]   # Firmware-Update, erhält NVS/LittleFS"
        exit 1
        ;;
esac

if [ ! -f "$IMAGE" ]; then
    echo "Fehlt: $IMAGE"
    echo "Zuerst ./build/build.sh ausführen."
    exit 1
fi

echo "Prüfe Build-Artefakte ..."
(
    cd "$OUT"
    sha256sum -c SHA256SUMS
)

echo
echo "Prüfe ESP32 auf $PORT ..."

if ! INFO="$(
docker run --rm \
    --device="$PORT:$PORT" \
    python:3.13-slim \
    sh -lc "pip install --quiet --no-cache-dir esptool==5.4.0 &&
            esptool --port '$PORT' flash-id" 2>&1
)"; then
    echo "$INFO"
    echo
    echo "ABBRUCH: Hardwareprüfung fehlgeschlagen."
    exit 1
fi

echo "$INFO"

if ! grep -q 'Detected flash size: 4MB' <<<"$INFO"; then
    echo
    echo "ABBRUCH: Kein 4-MB-Flash erkannt."
    exit 1
fi

echo
echo "Modus:  $MODE"
echo "Image:  $IMAGE"
echo "Offset: $OFFSET"

if [ "$MODE" = "install" ]; then
    echo
    echo "ACHTUNG: Erstinstallation überschreibt NVS."
    read -r -p "Zum Fortfahren INSTALL eingeben: " CONFIRM

    if [ "$CONFIRM" != "INSTALL" ]; then
        echo "Abgebrochen."
        exit 1
    fi
fi

echo
echo "Flashe $IMAGE nach $OFFSET ..."

docker run --rm \
    --device="$PORT:$PORT" \
    -v "$OUT:/out:ro" \
    python:3.13-slim \
    sh -lc "pip install --quiet --no-cache-dir esptool==5.4.0 &&
            esptool --chip esp32 --port '$PORT' write-flash \
            '$OFFSET' '/out/$(basename "$IMAGE")'"

if [ "$MODE" = "update" ]; then
    echo
    echo "Setze app0 als Bootpartition ..."

    docker run --rm \
        --device="$PORT:$PORT" \
        python:3.13-slim \
        sh -lc "pip install --quiet --no-cache-dir esptool==5.4.0 &&
                esptool --chip esp32 --port '$PORT' erase-region 0xe000 0x2000"
fi

echo
echo "Flash erfolgreich abgeschlossen."