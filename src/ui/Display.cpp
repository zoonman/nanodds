//
// Created by Philipp Tkachev on 2019-03-08.
//

#include "Display.h"



void Display::setTFT(Adafruit_ST7735 *t) {
    this->tft = t;
};

void Display::clear() {
    this->tft->fillScreen(ST7735_BLACK);
};

void Display::textxy(uint16_t x, uint16_t y, const char *text, uint16_t c, uint16_t b) {
    this->tft->setTextColor(c, b);
    this->tft->setCursor(x, y);
    this->tft->print(text);
}

void Display::textxy(uint16_t x, uint16_t y, String *text, uint16_t c, uint16_t b) {
    this->tft->setTextColor(c, b);
    this->tft->setCursor(x, y);
    this->tft->print(*text);
}

void Display::textxy(uint16_t x, uint16_t y, const __FlashStringHelper *ifsh, uint16_t c, uint16_t b) {
    this->tft->setTextColor(c, b);
    this->tft->setCursor(x, y);
    this->tft->print(ifsh);
}

void Display::drawTextBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, Message message, uint16_t c, uint16_t bg, TextAlignment alignment) {
    Bounds t = {};
    char text[35] = {};
    uint16_t offset = 0;
    uint8_t m = 0;
    for (; m < message; m++) {
        offset += (uint8_t) pgm_read_byte(
                MessageLengths + m
        );
    }
    yield();
    auto l = pgm_read_byte(MessageLengths + message);
    for (m = 0; m < min(l, 34); m++) {
        text[m] = pgm_read_byte(Messages + offset + m);
    }
    text[l] = '\0';
    yield();
    this->tft->getTextBounds(text, x, y, &t.x, &t.y, &t.w, &t.h);
    switch (alignment) {
        case AlignLeft:
            this->textxy(x, y + (h - t.h) / 2 + 1, text, c, bg);
            break;
        case AlignCenter:
            this->textxy(x + (w - t.w)/2, y + (h - t.h) / 2 + 1, text, c, bg);
            break;
        case AlignRight:
            this->textxy(x + w - t.w, y + (h - t.h) / 2 + 1, text, c, bg);
            break;
    }
};

void Display::drawRoundTextBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, Message message, uint16_t c, uint16_t bg) {
    this->tft->fillRoundRect(x, y, w, h, 2, bg);
    this->tft->drawRoundRect(x, y, w, h, 2, c);
    this->drawTextBox(x, y, w, h, message, c, bg, AlignCenter);
}

void Display::drawRoundTextBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, String *text, uint16_t c, uint16_t bg) {
    this->tft->fillRoundRect(x, y, w, h, 2, bg);
    this->tft->drawRoundRect(x, y, w, h, 2, c);
    Bounds t = {};
    this->tft->getTextBounds(*text, x, y, &t.x, &t.y, &t.w, &t.h);
    this->textxy(x + (w - t.w)/2, y + (h - t.h) / 2 + 1, text, c, bg);
}

Display::Display(Adafruit_ST7735 *tft) : tft(tft) {}
