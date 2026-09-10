#!/bin/bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/build/artifacts"

mkdir -p "$OUT"
rm -f "$OUT"/*.bin "$OUT"/SHA256SUMS

docker run --rm \
  -v "$ROOT:/src:ro" \
  -v "$OUT:/out" \
  python:3.13-slim \
  sh -lc '
    set -e
    mkdir /work
    cp -a /src/. /work/
    cd /work

    apt-get update
    apt-get install -y --no-install-recommends git
    rm -rf /var/lib/apt/lists/*

    pip install --quiet --no-cache-dir platformio==6.1.19
    pio run -e esp32

    cp .pio/build/esp32/firmware.bin /out/
    cp .pio/build/esp32/firmware_merged.bin /out/
    cp .pio/build/esp32/bootloader.bin /out/
    cp .pio/build/esp32/partitions.bin /out/
    cp release/esp32_jarolift_ota_*.bin /out/
    cp release/esp32_jarolift_flash_*.bin /out/

    cd /out
    sha256sum *.bin > SHA256SUMS
  '

echo
echo "Build-Artefakte:"
ls -lh "$OUT"
echo
cat "$OUT/SHA256SUMS"
