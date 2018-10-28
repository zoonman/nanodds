//
// Created by Philipp Tkachev on 2018-10-25.
//

#ifndef NANODDS_BANDS_H
#define NANODDS_BANDS_H

#include <stdint.h>
#include "screen.h"



// idea! smart band mode:
// when we reach end of the band it jumps to the beggining of the next ones

class Bands {
public:
    Bands() = default;

    const uint8_t total = BANDS;

    void render();

    void update();

    void next() {
        int8_t newBand;
        if (state.band < total - 1) {
            newBand = state.band + (uint8_t)1;
        } else {
            newBand = 0;
        }
        state.frequency = (static_cast<uint32_t>(BandsBounds[newBand].start + (BandsBounds[newBand].width / 2))) * 1000;
    }

private:
    bool smart = false;
};

void Bands::render() {
    if (state.band == total) {
        drawRoundTextBox(TFT_WIDTH - TFT_QUOTER_WIDTH, 0, TFT_QUOTER_WIDTH, 15, "AIR", COLOR_BRIGHT_RED, COLOR_DARK_RED);
    } else {
        itoa(BandsBounds[state.band].id, b, 10);
        drawRoundTextBox(TFT_WIDTH - TFT_QUOTER_WIDTH, 0, TFT_QUOTER_WIDTH, 15, b, COLOR_BRIGHT_GREEN, COLOR_DARK_GREEN);
    }
    tft.fillRect(0, SCALE_Y - 2, TFT_WIDTH-1, SCALE_T * 2, ST77XX_BLACK);
}

void Bands::update() {
    uint8_t i;
    for (i = 0; i < total; i++) {
        if (state.frequency/1000 >= BandsBounds[i].start && state.frequency/1000 <= BandsBounds[i].start + BandsBounds[i].width) {
            if (state.band != i) {
                state.band = i;
                render();
            }
            break;
        }
    }
    if (i == total && state.band != i) {
        state.band = i;
        render();
    }
}

#endif //NANODDS_BANDS_H
