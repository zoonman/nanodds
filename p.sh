#!/usr/bin/env bash

avrdude -p m1284p -c avrisp2 -P usb:16:79 -e -U flash:w:.pioenvs/atmega1284/firmware.hex -v