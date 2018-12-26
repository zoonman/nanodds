#!/usr/bin/env bash

P='avrdude -p m1284p -c avrispmkII -P usb:16:79 -e -U flash:w:.pioenvs/atmega1284/firmware.elf:e -v'

until $P; do
    ls
done