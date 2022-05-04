#!/bin/bash
BUILD_DIR=/tmp/open-tas-controller-build

mkdir -p "$BUILD_DIR"
cmake -S . -B "$BUILD_DIR"
cmake --build "$BUILD_DIR"

# thanks to https://github.com/ConorShore/RPi_Pico_Autoloader
# for the framework for this section
PICO_FOLDER=
while [ -z "$PICO_FOLDER" ] || [ ! -d "$PICO_FOLDER" ]; do
    PICO_FOLDER=$(find /run/media/*/RPI-RP2 -type d 2>/dev/null)
    sleep 1
done

cp "$BUILD_DIR/open-tas-controller.uf2" $PICO_FOLDER
