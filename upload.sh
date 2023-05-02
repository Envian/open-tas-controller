#!/bin/bash

# thanks to https://github.com/ConorShore/RPi_Pico_Autoloader
# for the framework for this section

echo 
echo "Put your pico into USB Mode"
echo

PICO_FOLDER=
while [ -z "$PICO_FOLDER" ] || [ ! -d "$PICO_FOLDER" ]; do
    PICO_FOLDER=$(find /run/media/*/RPI-RP2 -type d 2>/dev/null)
    sleep 1
done

echo "pico found: $PICO_FOLDER"
cp "$1/open-tas-controller.uf2" "$PICO_FOLDER"
