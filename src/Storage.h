//
// Created by Philipp Tkachev on 2019-03-09.
//
#include <Wire.h>

#ifndef NANODDS_STORAGE_H
#define NANODDS_STORAGE_H

#include "State.h"
// Memory Layout
// first 1 byte - Layout version
// 4kb - Settings
// 4kb - Current State
// 64kb - Place to store cells
// rest is reserved


class Storage {
    const size_t settingsAddr = 0x0010;
    const size_t currentStateAddr = 0x1000;
    const size_t cellsAddr = 0x2000;
public:
    Storage(uint8_t deviceAddress);

    void saveState(volatile State *state, size_t cell);
    void loadState(volatile State *state, size_t cell);

private:
    byte read(size_t addr);
    void write(size_t addr, byte data);
    uint8_t deviceAddress;
};

#endif //NANODDS_STORAGE_H
