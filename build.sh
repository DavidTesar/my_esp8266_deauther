#!/usr/bin/env bash
#
# Build / flash helper for the ESP8266 Deauther fork.
#
# Usage:
#   ./build.sh                      # compile only, default board (env BOARD or DSTIKE_DEAUTHER_OLED_V3)
#   ./build.sh compile              # same as above
#   ./build.sh flash /dev/ttyUSB0   # compile then upload to the given serial port
#   ./build.sh monitor /dev/ttyUSB0 # open serial monitor at 115200 baud
#   BOARD=NODEMCU ./build.sh        # override the target board define
#
# Board defines live in esp8266_deauther/A_config.h (e.g. NODEMCU, WEMOS_D1_MINI,
# DSTIKE_DEAUTHER_OLED_V3, DSTIKE_DEAUTHER_WATCH_V2, ...). See utils/arduino-cli-compile.py
# for the full list.

set -euo pipefail

# Make a locally-installed arduino-cli reachable.
export PATH="$HOME/.local/bin:$PATH"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SKETCH_DIR="$SCRIPT_DIR/esp8266_deauther"
BUILD_DIR="$SCRIPT_DIR/build"
FQBN="deauther:esp8266:generic"
BOARD="${BOARD:-DSTIKE_DEAUTHER_OLED_V3}"
BAUD="115200"

ACTION="${1:-compile}"
PORT="${2:-}"

compile() {
  echo ">> Compiling for board define: $BOARD"
  mkdir -p "$BUILD_DIR"
  arduino-cli compile \
    --fqbn "$FQBN" \
    --build-property "build.extra_flags=-DESP8266 -D${BOARD}" \
    --output-dir "$BUILD_DIR" \
    "$SKETCH_DIR"
  echo ">> Binary: $BUILD_DIR/esp8266_deauther.ino.bin"
}

case "$ACTION" in
  compile)
    compile
    ;;
  flash)
    [ -n "$PORT" ] || { echo "ERROR: pass a serial port, e.g. ./build.sh flash /dev/ttyUSB0"; exit 1; }
    compile
    echo ">> Uploading to $PORT"
    arduino-cli upload -p "$PORT" --fqbn "$FQBN" --input-dir "$BUILD_DIR" "$SKETCH_DIR"
    echo ">> Done. Open the serial monitor with: ./build.sh monitor $PORT"
    ;;
  monitor)
    [ -n "$PORT" ] || { echo "ERROR: pass a serial port, e.g. ./build.sh monitor /dev/ttyUSB0"; exit 1; }
    arduino-cli monitor -p "$PORT" --config "baudrate=$BAUD"
    ;;
  *)
    echo "Unknown action: $ACTION (use: compile | flash <port> | monitor <port>)"
    exit 1
    ;;
esac
