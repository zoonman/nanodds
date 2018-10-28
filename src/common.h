//
// Created by Philipp Tkachev on 2018-10-26.
//

#ifndef NANODDS_COMMON_H
#define NANODDS_COMMON_H

#include <stdint.h>

#define SCALE_Y               (uint8_t)(TFT_HEIGHT / 4 * 3)
#define SCALE_T               (uint8_t)3
#define START_F       (uint32_t)21150000
#define STR_BUFFER_SIZE       12


// positions
#define FREQUENCY_X           (uint8_t)10
#define FREQUENCY_Y           (uint8_t)(TFT_HEIGHT / 2)
#define FREQUENCY_FAKE_SPACE  '/'


char b[STR_BUFFER_SIZE];

#define BANDS                 8

#pragma pack(push)
enum Mode {CW = 0, LSB, USB, AM, FM};
const char*  ModeNames[] = {"CW", "LSB", "USB", "AM", "FM"};
#pragma pack(pop)

// volatile keyword must be used for global variables used inside interrupt handlers
// @see https://barrgroup.com/Embedded-Systems/How-To/C-Volatile-Keyword

volatile uint32_t oFrequency = 0; // Hz

// State will be used to save current active state
volatile struct State {
    uint32_t frequency = 0;
    uint32_t step = 100;
    bool isRIT = false;
    uint16_t RITFrequency = 0;
    uint8_t band = 0;
    Mode mode = LSB;
    bool tx = false;
} state;

// Memory Cells
struct Memory {
    State last;
    State cells[10];
};

uint32_t p10(uint8_t i) {
    if (i < 1) return 1;
    return 10 * p10(i - (uint8_t)1);
}


// Bands
struct BandRecord {
    uint8_t id; // meters
    uint16_t start; // kHZ
    uint16_t width; // kHz
};

#pragma pack(push,1)
const BandRecord BandsBounds[BANDS] = {
        {160, 1800, 200},
        {80,  3500, 500},
        {40,  7000, 300},
        {30, 10100, 150},
        {20, 14000, 350},
        {17, 18068, 100},
        {15, 21000, 450},
        {10, 28000, 1700}
};
#pragma pack(pop)

#endif //NANODDS_COMMON_H
