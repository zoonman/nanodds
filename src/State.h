//
// Created by Philipp Tkachev on 2019-03-09.
//

#include <Arduino.h>
#include <stdint.h>

#include "config.h"
#include "Mode.h"
#include "AppSettings.h"

#ifndef NANODDS_STATE_H
#define NANODDS_STATE_H




/**
 * State will be used to save current active state
 */
struct State {
    uint32_t frequency = 3750000;

    /**
     * @deprecated Use settings instead
     * */
    // uint32_t iFrequency = INTERMEDIATE_FREQUENCY;// max:-5.69dB 7.998928MHz
    /**
     * @deprecated
     * */
    // int32_t ssbOffset = 1500;
    /**
     * @deprecated Use settings instead
     * */
    // uint32_t cwOffset = 800;


    uint32_t altFrequency = 1600000;
    uint32_t step = 100;
    /**
     *  Receiver Incremental Tuning
     */
    bool isRIT = false;
    /**
     *  Receiver Incremental Tuning Frequency
     */
    int16_t RITFrequency = 0;
    /**
     * Band Index
     */
    uint8_t band = 0;
    /**
     * Current active mode
     */
    Mode mode = LSB;
    VFOType vfo = VFO_A;
    /**
     * Tx?
     */
    bool tx = false;
    bool isAltFrequency = false;
    /**
 * @deprecated Use settings instead
 * */
    uint8_t wpm = 10;
    uint8_t swr = 1;
    /**
 * @deprecated Use settings instead
 * */
    bool isPanoEnabled = true;
};

State StateFromVolatile(State volatile const& o);
void SetVolatileState(State volatile & r, State o);

struct MemoryStateCell {
    State state;
    uint32_t crc{};
};

// Memory Cells
struct Memory {
    State last;
    State cells[10];
};

#endif //NANODDS_STATE_H
