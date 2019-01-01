
//
// Created by Philipp Tkachev on 2018-12-25.
//
#include "bands.h"

#ifndef NANODDS_PANO_H
#define NANODDS_PANO_H

    #ifndef WATERFALL_ROWS
        #define WATERFALL_ROWS  22
    #endif

    #ifndef WATERFALL_COLS
        #define WATERFALL_COLS 160
    #endif

class Pano {
public:
    explicit Pano(uint8_t pin) {
        this->pin = pin;
    }


    void setup() {
        // just a stub
    }

    uint32_t loop() {


        uint16_t fStep; // kHz
        uint32_t panoFreq;

        if (state.band != BANDS) {
            fStep = BandsBounds[state.band].width * 1000 / WATERFALL_COLS;
            panoFreq = BandsBounds[state.band].start * 1000;
        } else {
            panoFreq = state.frequency - 100000;
            fStep = 625;
        }

        this->PXLT[this->col + WATERFALL_COLS * this->row] = analogRead(this->pin);
        if (++this->col >= WATERFALL_COLS) {
            this->col = 0;

            int8_t rowIndex = this->row;
            for (uint8_t y = 0; y < WATERFALL_ROWS; y++) {
                // render it
                for (uint8_t x = 0;x < WATERFALL_COLS; x++) {
                    tft.drawPixel(
                            x,
                            PANO_Y + WATERFALL_ROWS - y,
                            this->convertColor(this->PXLT[x + WATERFALL_COLS * rowIndex])
                    );
                }
                if (--rowIndex == -1) {
                    rowIndex = WATERFALL_ROWS - 1;
                };
            }
            if (++this->row >= WATERFALL_ROWS) {
                this->row = 0;
            }
        }

        panoFreq = panoFreq + fStep * this->col;
        return panoFreq;
    }

    uint16_t convertColor(uint16_t c) {
        uint8_t red = 0;
        uint8_t green =0;
        uint8_t blue = 0;

        if (c > 768) {
            // red
            red = (c - 768) / 8;
            green = 32 - (c - 768) / 8;

        } else if (c > 512) {
            // yellow
            green = (c - 512) / 8;
            red = (c - 512) / 8;
        } else if (c > 256) {
            // green
            green = (c - 256) / 8;
        } else {
            // blue
            blue = c / 8;
        }

        return ((red) << 15) | ((green) << 5) | (blue);
    }

private:
    uint8_t pin;
    volatile uint16_t PXLT[WATERFALL_COLS * WATERFALL_ROWS] = {0};

    volatile uint8_t row = 0, col = 0;

};
#endif //NANODDS_PANO_H
