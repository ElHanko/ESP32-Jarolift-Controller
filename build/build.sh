#!/bin/bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/build/artifacts"

usage() {
  echo "Usage: $0 [release]" >&2
}

case "$#" in
  0)
    MODE="personal"
    ;;
  1)
    if [[ "$1" != "release" ]]; then
      usage
      exit 2
    fi
    MODE="release"
    ;;
  *)
    usage
    exit 2
    ;;
esac

if [[ "$MODE" == "personal" ]]; then
  if [[ ! -f "$ROOT/include/local_secrets.h" ]]; then
    echo "Missing include/local_secrets.h - copy local_secrets.example.h and set private values." >&2
    exit 1
  fi
  echo "Build mode: personal (using include/local_secrets.h)"
else
  if [[ ! -f "$ROOT/include/local_secrets.example.h" ]]; then
    echo "Missing include/local_secrets.example.h." >&2
    exit 1
  fi
  echo "Build mode: release (using public default secrets)"
fi

mkdir -p "$OUT"
rm -f "$OUT"/*.bin "$OUT"/SHA256SUMS
rm -rf "$OUT/web"

DOCKER_ARGS=(
  --rm
  -e BUILD_MODE="$MODE"
  -v "$ROOT:/src:ro"
  -v "$OUT:/out"
)

if [[ "$MODE" == "release" ]]; then
  DOCKER_ARGS+=(
    -v "$ROOT/include/local_secrets.example.h:/src/include/local_secrets.h:ro"
  )
fi

docker run "${DOCKER_ARGS[@]}" \
  python:3.13-slim \
  sh -lc '
    set -e
    mkdir /work
    cp -a /src/. /work/
    cd /work

    if [ "$BUILD_MODE" = "release" ]; then
      cp include/local_secrets.example.h include/default_local_secrets.h
      cp include/default_local_secrets.h include/local_secrets.h
    elif [ ! -f include/local_secrets.h ]; then
      echo "Missing include/local_secrets.h." >&2
      exit 1
    fi

    apt-get update
    apt-get install -y --no-install-recommends git
    rm -rf /var/lib/apt/lists/*

    pip install --quiet --no-cache-dir platformio==6.1.19
    pio run -e esp32

    cp .pio/build/esp32/firmware.bin /out/
    cp .pio/build/esp32/firmware.factory.bin /out/firmware_merged.bin
    cp .pio/build/esp32/bootloader.bin /out/
    cp .pio/build/esp32/partitions.bin /out/

    if [ "$BUILD_MODE" = "release" ]; then
      VERSION=$(sed -n '\''s/^#define VERSION "\([^"]*\)".*/\1/p'\'' include/config.h)
      if [ -z "$VERSION" ]; then
        echo "Could not read VERSION from include/config.h." >&2
        exit 1
      fi
      cp .pio/build/esp32/firmware.bin "/out/esp32_jarolift_ota_${VERSION}.bin"
      cp .pio/build/esp32/firmware.factory.bin "/out/esp32_jarolift_flash_${VERSION}.bin"
    fi

    cp -a web/output /out/web

    cd /out
    sha256sum *.bin > SHA256SUMS
  '

echo
echo "Build-Artefakte:"
ls -lh "$OUT"
echo
cat "$OUT/SHA256SUMS"
