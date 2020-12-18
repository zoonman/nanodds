#include "SMeter.h"

void SMeter::loop()  {
    int d = analogRead(this->pin);
    if (this->pd != d) {
        this->pd = d;
        auto str1 = String(pd);

        display->textxy(10, RIT_Y, &str1, ST7735_ORANGE, ST7735_BLACK);

        if (this->pd > this->pdMax) {
            this->pdMax =this->pd;
        }
        if (this->pd < this->pdMin) {
            this->pdMin = this->pd;
        }

        // n < d < x
        // 1 < ? < 12

        auto d = (this->pdMax - this->pdMin) / 12.0;
        auto l = (this->pd - this->pdMin) / d;
        auto ld = static_cast<uint8_t>(l);
        this->drawLevel(ld);
        this->isRedrawForced = true;
    }
    this->draw();
}


void SMeter::setup() {
    // add pin listener
    pinMode(this->pin, INPUT);

    // render scale
    this->draw();
}


void SMeter::draw() {
    if (!this->isVisible) {
        return;
    }
    // display bars
    this->drawLevel(1);
    this->drawLevel(12);
    //
}

uint16_t SMeter::level2color(uint8_t l) {
    switch (l) {
        case 10:
            return ST77XX_YELLOW;
        case 11:
            return ST77XX_ORANGE;
        case 12:
            return ST77XX_RED;
        default:
            return ST77XX_GREEN;
    }
}

void SMeter::drawLevelBar(uint8_t l) {
    uint16_t color = this->level2color(l);
    uint8_t wOffset = (l > 9) ? (uint8_t)2 : (uint8_t)0, sOffset = 0;
    uint8_t lxp = (l - (uint8_t)1) * BAR_WIDTH + (uint8_t)2;

    if (l == 11) sOffset = 2;

    this->display->tft->fillRect(lxp + sOffset, TFT_HEIGHT - 8, BAR_WIDTH - 1 + wOffset, 6, color);
    char b[12];
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
    this->display->textxy(lxp + 4, TFT_HEIGHT - 18, b, color, ST77XX_BLACK);
}

void SMeter::drawLevel(uint8_t l) {
    this->display->tft->drawRect(0, TFT_HEIGHT - 10, TFT_WIDTH, 10, COLOR_BAND_BACKGROUND);
    if (l == pl) return;
    if (l > 12) l = 12;
    if (l < 1) l = 1;
    uint8_t i;
    if (l > pl) {
        for (i = pl; i < l; i++) {
            this->drawLevelBar(i);
        }
        pl = l;
    }
    if (l < pl) {
        for (i = pl; i > l; i--) {
            this->display->tft->fillRect((i - 1) * BAR_WIDTH + (i == 11 ? 4 : 2), TFT_HEIGHT - 8, BAR_WIDTH + (i > 9 ? 2 : 0), 6, ST77XX_BLACK);
        }
        pl = l;
    }
}