//
// Created by Philipp Tkachev on 2019-03-09.
//
#include <Wire.h>
#include "State.h"

#ifndef NANODDS_STORAGE_H
#define NANODDS_STORAGE_H

// Memory Layout
// first 1 byte - Layout version
// 4kb - Settings
// 4kb - Current State
// 64kb - Place to store cells
// rest is reserved

class Storage {
public:
    Storage(uint8_t deviceAddress);

    void saveState(State *state, size_t cell);
    void loadState(State *state);

private:
    byte read(size_t addr);
    void write(size_t addr, byte data);
    uint8_t deviceAddress;
};

#endif //NANODDS_STORAGE_H
