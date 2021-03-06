//
// Created by Philipp Tkachev on 2019-03-09.
//

#include "Storage.h"

Storage::Storage(uint8_t deviceAddress) : deviceAddress(deviceAddress) {
    //Wire.begin(deviceAddress);
}

byte Storage::read(size_t addr) {
    byte data = 0x00;
    Wire.beginTransmission(this->deviceAddress);
    Wire.write(addr>>8);
    Wire.write(addr & 0xFF);
    Wire.endTransmission();

    Wire.requestFrom(deviceAddress, 1u);
    if (Wire.available()) {
        int result = Wire.read();
        if (result > 0) {
            return static_cast<byte>(result & 0xFF);
        }
    }
    return data;
}

void Storage::write(size_t addr, byte data) {
    Wire.beginTransmission(this->deviceAddress);
    Wire.write(addr>>8);
    Wire.write(addr & 0xFF);
    Wire.write(data);
    Wire.endTransmission();
    delay(5);// let's wait a moment
}

void Storage::saveState(volatile State *state, size_t cell) {
    auto * ptr = (unsigned char *)&state;
    for (size_t a = 0;a < sizeof(State);a++) {
        byte b;
        b = *ptr;
        this->write(this->currentStateAddr + a, b);
        ptr++;
    }
}

void Storage::loadState(volatile State *state, size_t cell) {
    /**/
    for (size_t a = 0;a < sizeof(State);a++) {
        auto * ptr = (unsigned char *)&state + a;
        // *ptr = this->read(this->currentStateAddr + a);

    }
}