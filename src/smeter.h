//
// Created by Philipp Tkachev on 2018-10-25.
//

#ifndef NANODDS_LEVELS_H
#define NANODDS_LEVELS_H

#define BAR_WIDTH             (uint8_t)14

#include "screen.h"

class SMeter {
public:
    SMeter(uint8_t pin) {
        d = 1;
        pd = 1;
        this->pin = pin;
    };

    void setup() {
        drawLevel(12);
    }

    void drawLevelBar(uint8_t l) {
        uint16_t color;
        uint8_t wOffset = (l > 9) ? (uint8_t)2 : (uint8_t)0, sOffset = 0;
        uint8_t lxp = (l - (uint8_t)1) * BAR_WIDTH + (uint8_t)2;

        switch (l) {
            case 10: color = ST77XX_YELLOW; break;
            case 11: color = ST77XX_ORANGE; sOffset = 2; break;
            case 12: color = ST77XX_RED;  break;
            default:
                color = ST77XX_GREEN;
        }

        tft.fillRect(lxp + sOffset, TFT_HEIGHT - 8, BAR_WIDTH - 1 + wOffset, 6, color);

        if (l > 9) {
            itoa(l+1, b, 10);
            b[0] = '+';
            b[2] = '0';
            b[3] = 0;
            lxp -= 7;
            if (l > 10) {
                lxp += 3;
            }
        } else {
            itoa(l, b, 10);
        }
        textxy(lxp + 4, TFT_HEIGHT - 18, b, color, ST77XX_BLACK);
    }

    void drawLevel(uint8_t l) {
        tft.drawRect(0, TFT_HEIGHT - 10, TFT_WIDTH, 10, COLOR_BAND_BACKGROUND);
        if (l == pl) return;
        if (l > 12) l = 12;
        if (l < 1) l = 1;
        uint8_t i;
        if (l > pl) {
            for (i = pl; i < l; i++) {
                drawLevelBar(i);
            }
            pl = l;
        }
        if (l < pl) {
            for (i = pl; i > l; i--) {
                tft.fillRect((i - 1) * BAR_WIDTH + (i == 11 ? 4 : 2), TFT_HEIGHT - 8, BAR_WIDTH + (i > 9 ? 2 : 0), 6, ST77XX_BLACK);
            }
            pl = l;
        }
    }

    void loop() {
        int d = analogRead(pin);
        if (pd != d) {
            itoa(d, b, 10);
            tft.setTextColor(COLOR_BRIGHT_BLUE);
            textxy(TFT_WIDTH - 30, TFT_HEIGHT - 30, b, COLOR_BRIGHT_BLUE, ST77XX_BLACK);
            pd = d;
            drawLevel(static_cast<uint8_t>(pd / 92));
        }
    }
private:
    volatile uint8_t pl = 1;
    int d,pd;
    uint8_t pin;
};

#endif //NANODDS_LEVELS_H
