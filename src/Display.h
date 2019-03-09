//
// Created by Philipp Tkachev on 2019-03-08.
//

#ifndef NANODDS_DISPLAY_H
#define NANODDS_DISPLAY_H

#include <Adafruit_ST7735.h>
#include "colors.h"

struct Bounds {
    int16_t x, y;
    uint16_t w, h;
};

class Display {
public:
    Display(Adafruit_ST7735 *tft);
    Adafruit_ST7735 *tft;
    void setTFT(Adafruit_ST7735 *tft);
    void textxy(uint16_t x, uint16_t y, String *text, uint16_t c, uint16_t b);
    void textxy(uint16_t x, uint16_t y, const __FlashStringHelper *ifsh, uint16_t c, uint16_t b);
    void drawRoundTextBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, String *text, uint16_t c, uint16_t bg);
    void drawRoundTextBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const __FlashStringHelper *ifsh, uint16_t c, uint16_t bg);
};


#endif //NANODDS_DISPLAY_H
