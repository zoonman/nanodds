//
// Created by Philipp Tkachev on 2018-10-26.
//

#ifndef NANODDS_COMMON_H
#define NANODDS_COMMON_H

#include <stdint.h>

#define SCALE_T               (uint8_t)5
#define START_F               (uint32_t)3899000
#define STR_BUFFER_SIZE       12


// positions
#define FREQUENCY_X           (uint8_t)10
#define FREQUENCY_Y           (uint8_t)56
#define FREQUENCY_FAKE_SPACE  '/'
#define INTERMEDIATE_FREQUENCY 8000000

char b[STR_BUFFER_SIZE];

#define BANDS                 8

#pragma pack(push)
enum Mode {
    CW = 0, LSB, USB, AM, FM
};
static const char *ModeNames[] = {"CW", "LSB", "USB", "AM", "FM"};
#pragma pack(pop)

// volatile keyword must be used for global variables used inside interrupt handlers
// @see https://barrgroup.com/Embedded-Systems/How-To/C-Volatile-Keyword

volatile uint32_t oFrequency = 0; // Hz

/**
 * State will be used to save current active state
 */
volatile struct State {
    uint32_t frequency = START_F;
    uint32_t altFrequency = START_F;
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
     * If the word PARIS can be sent ten times in a minute using normal Morse code timing then the code speed is 10 WPM.
     *
     * dot_ms = 60 * 1000 / (wpm * 50) = 1200 / wpm
     */
    uint8_t wpm = 10;
    uint8_t swr = 1;
} state;

// Memory Cells
struct Memory {
    State last;
    State cells[10];
};

uint32_t p10(uint8_t i) {
    if (i < 1) return 1;
    return 10 * p10(i - (uint8_t) 1);
}


// Bands
struct BandRecord {
    uint8_t id; // meters
    uint16_t start; // kHZ
    uint16_t width; // kHz
};

#pragma pack(push, 1)
const BandRecord BandsBounds[BANDS] = {
        {160, 1800,  200},
        {80,  3500,  500},
        {40,  7000,  300},
        {30,  10100, 150},
        {20,  14000, 350},
        {17,  18068, 100},
        {15,  21000, 450},
        {10,  28000, 1700}
};
#pragma pack(pop)

#endif //NANODDS_COMMON_H
