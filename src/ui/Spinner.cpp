//
// Created by Philipp Tkachev on 2019-05-20.
//

#include "Spinner.h"


void Spinner::draw() {
    if (!this->isVisible) {
        return;
    }
    String text;
    text.concat(this->value);
    this->display->textxy(this->left, this->top, this->label, this->color, ST7735_BLACK);
    this->display->drawRoundTextBox(
            this->left,
            this->top,
            this->width,
            this->height,
            &text,
            this->color,
            ST7735_BLACK
    );
}

Spinner::Spinner(Display *display) {
    this->step = 1;
    this->display = display;
    this->left = 0;
    this->top = 0;
    this->width = static_cast<uint8_t>(this->display->tft->width());
    this->height = 22;
    this->color = COLOR_GRAY_MEDIUM;
    this->isFocused = false;
}

void Spinner::inc() {
    this->value += this->step;
    this->isRedraw = true;
}

void Spinner::dec() {
    this->value -= this->step;
    this->isRedraw = true;
}

int Spinner::getValue() {
    return this->value;
}

void Spinner::setValue(int value) {
    this->value = value;
    this->isRedraw = true;
}

void Spinner::loop() {
    if (this->isRedraw) {
        this->draw();
    }
}