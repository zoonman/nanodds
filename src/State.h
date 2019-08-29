//
// Created by Philipp Tkachev on 2019-03-09.
//

#include <Arduino.h>
#include <stdint.h>

#include "config.h"
#include "Mode.h"


#ifndef NANODDS_STATE_H
#define NANODDS_STATE_H


/**
 * State will be used to save current active state
 */
struct State {
    uint32_t frequency = 0;
    uint32_t iFrequency = INTERMEDIATE_FREQUENCY;// max:-5.69dB 7.998928MHz

    uint32_t ssbOffset = 1500;
    uint32_t cwOffset = 800;
    uint32_t altFrequency = 0;
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
    /**
     * Tx?
     */
    bool tx = false;
    bool isAltFrequency = false;
    /**
     * CW Words per minute
     *
     * The basic element of Morse code is the dot and all other elements
     * can be defined in terms of multiples of the dot length.
     * The word PARIS is used because this is the length of a typical word
     * in English plain text, it has a total length of 50 dot lengths.
     * If the word PARIS can be sent ten times in a minute using
     * normal Morse code timing then the code speed is 10 WPM.
     *
     * dot_ms = 60 * 1000 / (wpm * 50) = 1200 / wpm
     */
    uint8_t wpm = 10;
    uint8_t swr = 1;
};


// Memory Cells
struct Memory {
    State last;
    State cells[10];
};

#endif //NANODDS_STATE_H
