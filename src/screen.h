//
// Created by Philipp Tkachev on 2018-10-26.
//

#ifndef NANODDS_SCREEN_H
#define NANODDS_SCREEN_H

#include "Adafruit_GFX.h"
#include <Adafruit_ST7735.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <avr/pgmspace.h>

#include "common.h"

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
// #define TFT_CS                10 // D10, Chip select pin #, SS
//  #define TFT_RST               8  // D8, Reset pin # (optional, pass -1 if unused), PB0
// #define TFT_DC                9  // A0, Data/Command pin # (PB1)

#define TFT_CS                10 // D10, Chip select pin #, SS
#define TFT_RST               4  // D8, Reset pin # (optional, pass -1 if unused), PB0
#define TFT_DC                5  // A0, Data/Command pin # (PB1)


// colors
#define COLOR_BRIGHT_GREEN    0x96C0
#define COLOR_DARK_GREEN      0x29C0
#define COLOR_DARK_RED        0x6800
#define COLOR_MEDIUM_RED      0x9041
#define COLOR_BRIGHT_RED      0xE800
#define COLOR_BRIGHT_BLUE     0x1473
#define COLOR_BAND_BACKGROUND 0x0208
#define COLOR_GRAY_MEDIUM     0x9CD3
#define COLOR_DARK_GRAY     0x31a6


// display
#define TFT_HEIGHT            (uint8_t)ST7735_TFTHEIGHT_128
#define TFT_WIDTH             (uint8_t)160
#define TFT_QUOTER_WIDTH      (uint8_t)(TFT_WIDTH/4)
#define GRID                  (uint8_t)(TFT_HEIGHT/16) // 8
#define SCALE_Y               (uint8_t)(GRID*10)
#define PANO_Y                (uint8_t)(GRID*11 - 3)
#define STEP_Y                (uint8_t)(GRID*3)
#define RIT_Y                 (uint8_t)(GRID*8)
#define SWR_SCALE_Y           (uint8_t)(GRID*10)
#define SWR_SCALE_TY          (uint8_t)(SWR_SCALE_Y - GRID)

struct Bounds {
    int16_t x, y;
    uint16_t w, h;
};

// Init Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
volatile uint16_t scalePosX = 0;


void textxy(uint16_t x, uint16_t y, const char *text) {
    tft.setCursor(x, y);
    tft.print(text);
}

void textxy(uint16_t x, uint16_t y, const char *text, uint16_t c, uint16_t b) {
    tft.setTextColor(c, b);
    textxy(x, y, text);
}

void drawRoundTextBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char *text, uint16_t c, uint16_t bg) {
    tft.fillRoundRect(x, y, w, h, 2, bg);
    tft.drawRoundRect(x, y, w, h, 2, c);
    Bounds t = {};
    tft.getTextBounds(text, x, y, &t.x, &t.y, &t.w, &t.h);
    textxy(x + (w - t.w)/2, y + (h - t.h) / 2 + 1, text, c, bg);
}

char oldFreq[STR_BUFFER_SIZE] = "\0";


uint8_t symbolLength(char c) {
    switch (c) {
        case '.': return 7;
        case '/': return 6;
        default: return 16;
    }
}

void displayFrequency() {
    char cFreq[STR_BUFFER_SIZE] = "\0";
    char dFreq[STR_BUFFER_SIZE] = "\0";
    ultoa(state.frequency, cFreq, 10);
    int8_t j = STR_BUFFER_SIZE - 2;
    for (int8_t i = STR_BUFFER_SIZE - 2; i >= 0; i--) {
        while (cFreq[j] == 0 && j > 0) {
            j--;
        };
        if (i == 7) {
            dFreq[i] = FREQUENCY_FAKE_SPACE;
        } else
        if (i == 3) {
            dFreq[i] = '.';
        } else
        if (j >= 0) {
            dFreq[i] = cFreq[j];
            j--;
        } else {
            dFreq[i] = FREQUENCY_FAKE_SPACE;
        }
    }
    tft.setTextSize(1);
    tft.setFont(&FreeSansBold12pt7b);

    int16_t xof = FREQUENCY_X + (state.frequency < 1E+7 ? 10 : 0);
    for (int8_t i = 0; i < STR_BUFFER_SIZE-1; i++) {
        auto osl = symbolLength(oldFreq[i]);
        auto nsl = symbolLength(dFreq[i]);
        if (oldFreq[i] != dFreq[i]) {
            if (osl != nsl) {
                // we have a shift
                Bounds t = {};
                tft.getTextBounds(oldFreq, FREQUENCY_X, FREQUENCY_Y, &t.x, &t.y, &t.w, &t.h);
                tft.fillRect(t.x, t.y, t.w, t.h, ST77XX_BLACK);
                break;
            } else {
                tft.fillRect(xof, FREQUENCY_Y-21, osl, 22, ST77XX_BLACK);
            }
        }
        xof += osl ;
    }

    textxy(
            FREQUENCY_X + (state.frequency < 1E+7 ? 10 : 0),
            FREQUENCY_Y,
            dFreq,
            COLOR_BRIGHT_GREEN,
            ST77XX_BLACK
            );

    strcpy(oldFreq, dFreq);
    tft.setFont();
    textxy(0, STEP_Y, ("Frequency:"), COLOR_GRAY_MEDIUM, ST77XX_BLACK);
}



void scaleTriangle(uint16_t c) {
    tft.fillTriangle(scalePosX, SCALE_Y - 1, scalePosX - SCALE_T, SCALE_Y - SCALE_T - 1, scalePosX + SCALE_T, SCALE_Y - 1 - SCALE_T, c);
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

void displayVFO() {
    drawRoundTextBox(
        TFT_QUOTER_WIDTH + 1,
        0,
        TFT_QUOTER_WIDTH-2,
        15,
        (state.isAltFrequency ? "VFO B" : "VFO A"),
        COLOR_GRAY_MEDIUM,
        COLOR_DARK_GREEN
    );
}

void changeFrequencyStep(int8_t offset) {
    if (offset < 0 && state.step > 1) {
        state.step /= 10;
    } else if (offset > 0) {
        state.step = state.step < 1E+6 ? state.step * 10 : 1;
    }

    textxy(80, STEP_Y, ("Step: "), COLOR_GRAY_MEDIUM, ST77XX_BLACK);
    ultoa(state.step, b, 10);
    tft.fillRect(110, STEP_Y, 50, 12, ST77XX_BLACK);
    textxy(110, STEP_Y, b, COLOR_GRAY_MEDIUM, ST77XX_BLACK);
    tft.fillRect(0, TFT_HEIGHT / 2 + 3, TFT_WIDTH, 2, ST77XX_BLACK);
}

void displayWPM() {
    if (state.mode == CW) {
        char s[STR_BUFFER_SIZE] = "\0";
        ltoa(state.wpm, s, 10);
        textxy(0, RIT_Y, "WPM:", ST77XX_BLUE, ST77XX_BLACK);
        textxy(30, RIT_Y, s, ST77XX_BLUE, ST77XX_BLACK);
    }
}

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
    tft.drawFastVLine(swrx, SWR_SCALE_Y, 5, COLOR_GRAY_MEDIUM);
}


void intToStrFP(char *buf, uint8_t n, uint8_t fp, uint8_t length) {
    char s[5] = "\0";
    utoa(n, s, 10);
    auto l = strlen(s);
    auto bf = length - 2;
    buf[length - 1] = '\0';
    while (l > 0) {
        if (fp > 0 && length - 2 - bf == fp) {
            buf[bf--] = '.';
            continue;
        }
        l--;
        buf[bf] = s[l];
        bf--;

    }
    while (bf >= 0) buf[bf--] = ' ';
}

#define DELTA 1
void displaySWR() {
    // 1..1.5..2..3...inf
    uint8_t swr = (uint8_t)(analogRead(A1) / 4); // 10 = 1, 15 = 1.5, 20 = 2, etc, max is 255, 1024
    /**
     * SWR is calculated as ratio REFLECTED/DIRECT POWER
     */
    swr = swr > 10 ? swr : (uint8_t)10;

    if (swr > state.swr + DELTA || swr < state.swr - DELTA) {
        state.swr = swr;
        tft.drawFastVLine(0, SWR_SCALE_Y, 5, COLOR_GRAY_MEDIUM);
        tft.fillRect(0, SWR_SCALE_Y, TFT_WIDTH-1, 5, tft.color565(20, 20, 20));

        uint8_t swrX = convertSWR(state.swr);
        uint16_t color = COLOR_BRIGHT_GREEN;
        for (uint8_t x = 0; x < min(swrX, TFT_WIDTH); x+=2) {
            if (x < convertSWR(15)) {
                color = COLOR_BRIGHT_GREEN;
            } else if (x < convertSWR(30)) {
                color = ST77XX_YELLOW;
            } else {
                color = COLOR_BRIGHT_RED;
            }
            tft.drawFastVLine(x, SWR_SCALE_Y, 5, color);
        }
        textxy(0, SWR_SCALE_TY, ("SWR"), COLOR_GRAY_MEDIUM, ST77XX_BLACK);
        char swrb[8] = "\0";
        // utoa(state.swr, b, 10);
        intToStrFP(swrb, state.swr, 1, 5);
        textxy(20, SWR_SCALE_TY, swrb, color, ST77XX_BLACK);
        displaySWRTick(15, "1.5");
        displaySWRTick(20, "2");
        displaySWRTick(30, "3");
        displaySWRTick(50, "5");
        displaySWRTick(100, "10");
        tft.drawFastVLine(0, SWR_SCALE_Y, 6, COLOR_GRAY_MEDIUM);
        textxy(TFT_WIDTH - 20, SWR_SCALE_TY, ("inf"));
    }
}


void displayScale(bool redraw) {
    uint16_t fStep = 500; // kHz
    uint16_t fStart = 1000; // kHz
    uint16_t fWidth = 30000; // kHz
    if (state.band != BANDS) {
        fStep = 50;
        fStart = BandsBounds[state.band].start;
        fWidth = BandsBounds[state.band].width;
    }
    auto newScalePosX = static_cast<uint16_t>((state.frequency/1000 - fStart) * TFT_WIDTH / fWidth);
    if (scalePosX != newScalePosX || redraw) {
        // draw boundaries
        tft.drawFastHLine(0, SCALE_Y, TFT_WIDTH-1, COLOR_GRAY_MEDIUM);
        tft.drawFastVLine(0, SCALE_Y - 2, 5, COLOR_GRAY_MEDIUM);
        tft.drawFastVLine(TFT_WIDTH-1, SCALE_Y - 2, 5, COLOR_GRAY_MEDIUM);
        // draw ticks
        for (uint32_t f = 0; f < fWidth; f += fStep) {
            tft.drawFastVLine(
                    static_cast<uint16_t>((f * TFT_WIDTH) / fWidth),
                    SCALE_Y,
                    f % (fStep * 2) == 0 ? 4 : 2,
                    COLOR_GRAY_MEDIUM
            );
        }
        // draw pointer
        scaleTriangle(ST77XX_BLACK);
        scalePosX = newScalePosX;
        scaleTriangle(COLOR_BRIGHT_BLUE);
    }
}

#endif //NANODDS_SCREEN_H
