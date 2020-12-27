//
// Created by Philipp Tkachev on 2018-10-26.
//

#ifndef NANODDS_SCREEN_H
#define NANODDS_SCREEN_H

#include "config.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "common.h"
#include "colors.h"
#include "menu/Action.h"
#include <SPI.h>


// Init Display
Adafruit_ST7735 *tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

/**
 * @deprecated
 * @param x
 * @param y
 * @param text
 */
void textxy(uint16_t x, uint16_t y, const char *text) {
    tft->setCursor(x, y);
    tft->print(text);
}

/**
 * deprecated
 * @param x
 * @param y
 * @param text
 * @param c
 * @param b
 */
void textxy(uint16_t x, uint16_t y, const char *text, uint16_t c, uint16_t b) {
    tft->setTextColor(c, b);
    textxy(x, y, text);
}

/**
 * @deprecated
 * @param x
 * @param y
 * @param w
 * @param h
 * @param text
 * @param c
 * @param bg
 */
void drawRoundTextBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char *text, uint16_t c, uint16_t bg) {
    tft->fillRoundRect(x, y, w, h, 2, bg);
    tft->drawRoundRect(x, y, w, h, 2, c);
    Bounds t = {};
    tft->getTextBounds(text, x, y, &t.x, &t.y, &t.w, &t.h);
    textxy(x + (w - t.w)/2, y + (h - t.h) / 2 + 1, text, c, bg);
}


void displayMode() {
    if (state.tx) {
        drawRoundTextBox(0, 0, TFT_QUOTER_WIDTH, 15, ("TX"), ST77XX_WHITE, COLOR_MEDIUM_RED);
    } else {
        drawRoundTextBox(0, 0, TFT_QUOTER_WIDTH, 15, ("RX"), COLOR_BRIGHT_GREEN, COLOR_DARK_GREEN);
    }
}

void displayModulation() {
    drawRoundTextBox(
        TFT_WIDTH - TFT_QUOTER_WIDTH * (uint8_t)2, 0,
        TFT_QUOTER_WIDTH - (uint8_t)1,
        15,
        ModeNames[state.mode],
        COLOR_BRIGHT_BLUE,
        COLOR_BAND_BACKGROUND
    );
}


void displayWPM() {
    if (state.mode == CW) {
        char s[STR_BUFFER_SIZE] = "\0";
        ltoa(state.wpm, s, 10);
        textxy(0, RIT_Y, "WPM:", ST77XX_BLUE, ST77XX_BLACK);
        textxy(30, RIT_Y, s, ST77XX_BLUE, ST77XX_BLACK);
    }
}

/***
 * @deprecated use Frequency
 */
/*
void displayRIT() {
    char s[STR_BUFFER_SIZE] = "\0";
    if (state.isRIT) {
        char f[STR_BUFFER_SIZE] = "\0";
        ltoa(state.RITFrequency, f, 10);
        strcat(s, "RIT: ");
        size_t fl = strlen(f);
        if (fl < 5) memset(s + strlen(s), 0x20, 5 - fl);
        strcat(s, f);
    } else {
        memset(s, 0x20, 10);
    }
    textxy(100, RIT_Y, s, ST77XX_MAGENTA, ST77XX_BLACK);
}

void displaySMeter() {

}

uint8_t convertSWR(uint8_t swr) {
    return TFT_WIDTH - static_cast<uint8_t>(TFT_WIDTH/log(swr-10));
}

void displaySWRTick(uint8_t value, const char label[]) {
    uint8_t swrx = convertSWR(value);
    if (swrx % 2 == 0) {
        swrx++;
    }
    textxy(swrx - strlen(label) * 5 / 2, SWR_SCALE_TY, label, COLOR_GRAY_MEDIUM, ST77XX_BLACK);
    tft->drawFastVLine(swrx, SWR_SCALE_Y, 5, COLOR_GRAY_MEDIUM);
}
*/


#endif //NANODDS_SCREEN_H
