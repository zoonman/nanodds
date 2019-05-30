
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
    explicit Pano(uint8_t pin, Si5351 *pll) {
        this->pin = pin;
        this->pll = pll;
    }

    void setup() {
        // just a stub
    }

    uint32_t loop() {



        uint16_t fStep; // Hz
        uint32_t panoFreq;

        if (state.band != BANDS) {
            fStep = BandsBounds[state.band].width * 1000 / WATERFALL_COLS;
            panoFreq = BandsBounds[state.band].start * 1000;
        } else {
            panoFreq = state.frequency - 50000;
            fStep = 100000 / WATERFALL_COLS;
        }
        yield();
        cbi(PRR0, PRADC); // enable power reduction ADC
        ;
        int rv = analogRead(this->pin);
        yield();
        // debug
        sprintf(b, "%4d", rv);
        // textxy(0, TFT_HEIGHT /2, b, COLOR_BRIGHT_GREEN, ST77XX_BLACK);
        if (this->min) {
            //
        }
        //
        this->PXLT[this->col + WATERFALL_COLS * this->row] = this->convertColor(rv * 50);
        if (++this->col >= WATERFALL_COLS) {
            this->col = 0;

            int8_t rowIndex = this->row;
            // optimized pixel transfer over SPI
            tft.startWrite();
            for (uint8_t y = 0; y < WATERFALL_ROWS; y++) {
                // render it
                for (uint8_t x = 0;x < WATERFALL_COLS; x++) {
                    tft.writePixel(
                            x,
                            PANO_Y + WATERFALL_ROWS - y,
                            this->PXLT[x + WATERFALL_COLS * rowIndex]
                    );
                }
                if (--rowIndex == -1) {
                    rowIndex = WATERFALL_ROWS - 1;
                };
            }
            tft.endWrite();
            if (++this->row >= WATERFALL_ROWS) {
                this->row = 0;
            }
        }

        panoFreq = panoFreq + fStep * this->col;
        yield();
        this->pll->set_freq(
                static_cast<uint64_t>(panoFreq + 16000000) * 100ULL,
                SI5351_CLK2
        );
        yield();
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
    uint8_t min = 255;
    uint8_t max = 0;
    Si5351 *pll;
    volatile uint16_t PXLT[WATERFALL_COLS * WATERFALL_ROWS] = {0};

    volatile uint8_t row = 0, col = 0;

};
#endif //NANODDS_PANO_H
