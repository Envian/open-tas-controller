#!/bin/bash
PROJECT_DIR=$(dirname "$(readlink -f '$0')")
BUILD_DIR=/tmp/open-tas-controller-build
PIO_H_DIR=$PROJECT_DIR/pio.h/

mkdir -p "$BUILD_DIR"
mkdir -p "$PIO_H_DIR"
cd "$BUILD_DIR/"

cmake "$PROJECT_DIR"
make -i

cp -u *.pio.h "$PIO_H_DIR"

# thanks to https://github.com/ConorShore/RPi_Pico_Autoloader
# for the framework for this section
PICO_FOLDER=
while [ -z "$PICO_FOLDER" ] || [ ! -d "$PICO_FOLDER" ]; do
    PICO_FOLDER=$(find /run/media/*/RPI-RP2 -type d 2>/dev/null)
    sleep 1
done

cp open-tas-controller.uf2 $PICO_FOLDER
