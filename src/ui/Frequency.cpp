//
// Created by Philipp Tkachev on 2019-10-29.
//
#include "Frequency.h"

void Frequency::loop() {

    if (this->semif != this->state->frequency) {
        if (this->semif > this->state->frequency) {
            this->semif--;
        } else {
            this->semif++;
        }

        auto f = this->state->frequency +
                this->settings->iFrequency +
                (this->state->isRIT && !this->state->tx ? this->state->RITFrequency : 0);
        // @todo: add reverse in future
        this->pll->set_freq(
                static_cast<uint64_t>(f) * 100ULL,
                SI5351_CLK0
        );
    }
}

void Frequency::changeFrequencyStep(int8_t offset) {
    if (offset < 0 && this->state->step > 1) {
        this->state->step /= 10;
    } else if (offset > 0) {
        this->state->step = this->state->step < 1E+6 ? this->state->step * 10 : 1;
    }
}

void Frequency::draw() {
    if (!this->isVisible) {
        return;
    }
    switch (this->redrawType) {
        case Full:
            this->drawStep();
            this->drawRIT();

        case Data:
            ;
            break;
    }
    this->redrawType = No;
}

void Frequency::drawStep() {
    char b[12];
    this->display->textxy(80, STEP_Y, ("Step: "), COLOR_GRAY_MEDIUM, ST77XX_BLACK);
    ultoa(this->state->step, b, 10);
    this->display->tft->fillRect(110, STEP_Y, 50, 12, ST77XX_BLACK);
    this->display->textxy(110, STEP_Y, b, COLOR_GRAY_MEDIUM, ST77XX_BLACK);
    this->display->tft->fillRect(0, TFT_HEIGHT / 2 + 3, TFT_WIDTH, 2, ST77XX_BLACK);
}

void Frequency::drawRIT() {
    char s[12] = "\0";
    if (this->state->isRIT) {
        char f[12] = "\0";
        ltoa(this->state->RITFrequency, f, 10);
        strcat(s, "RIT: ");
        size_t fl = strlen(f);
        if (fl < 5) memset(s + strlen(s), 0x20, 5 - fl);
        strcat(s, f);
    } else {
        memset(s, 0x20, 10);
    }
    this->display->textxy(100, RIT_Y, s, ST77XX_MAGENTA, ST77XX_BLACK);
}