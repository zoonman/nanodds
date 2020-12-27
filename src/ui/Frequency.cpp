//
// Created by Philipp Tkachev on 2019-10-29.
//
#include "Frequency.h"

void Frequency::loop() {

    if (this->cachedStateFrequency != this->state->frequency) {
        this->updatePllFrequency();
        this->cachedStateFrequency = this->state->frequency;
        this->scheduleRedraw(Data);
    }
    this->draw();
}


void Frequency::updatePllFrequency() {
    uint64_t f1;
    uint64_t f2;
    int64_t fOffset;
    switch (state->mode) {
        case LSB:
            fOffset = settings->ssbOffset;
            break;
        case USB:
            fOffset = -settings->ssbOffset;
            break;
        case CW:
            fOffset = -settings->cwOffset;
            break;
        default:
            // AM & FM (not implemented)
            fOffset = 0;
    }

    // in a split mode we receive on one frequency and transmit on alternative frequency
    // normally we apply RIT offset in RX and transmit on a correct frequency
    if (VFO_SPLIT == state->vfo) {
        if (state->tx) {
            f1 = settings->iFrequency + fOffset;
            f2 = state->altFrequency + settings->iFrequency;
        } else {
            f1 = state->frequency + settings->iFrequency + (state->isRIT ? state->RITFrequency : 0);
            f2 = settings->iFrequency + fOffset;
        }
    } else {
        if (state->tx) {
            f1 = settings->iFrequency + fOffset;
            f2 = state->frequency + settings->iFrequency ;
        } else {
            f1 = state->frequency + settings->iFrequency + (state->isRIT ? state->RITFrequency : 0);
            f2 = settings->iFrequency + fOffset;
        }
    }

    this->pll->set_freq(
            static_cast<uint64_t>(f1) * 100ULL,
            SI5351_CLK0
    );
    yield();
    delay(1);
    this->pll->set_freq(
            static_cast<uint64_t>(f2) * 100ULL,
            SI5351_CLK1
    );
    yield();
    delay(1);
}

void Frequency::changeFrequencyStep(int8_t offset) {
    if (offset < 0 && this->state->step > 1) {
        this->state->step /= 10;
    } else if (offset > 0) {
        this->state->step = this->state->step < 1E+6 ? this->state->step * 10 : 1;
    }
    this->scheduleRedraw(Full);
}

void Frequency::draw() {
    if (!this->isVisible) {
        return;
    }
    switch (this->redrawType) {
        case Full:
            this->drawStep();
        case Data:
            this->drawFrequency();
            this->drawRIT();
            ;
            break;
    }
    this->redrawType = No;
}

void Frequency::drawStep() {
    char b[12];
    if (this->redrawType == Full) {
        this->display->textxy(80, STEP_Y, ("Step: "), COLOR_GRAY_MEDIUM, ST77XX_BLACK);
    }
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


char oldFreq[STR_BUFFER_SIZE] = "\0";

uint8_t symbolLength(char c) {
    switch (c) {
        case '.': return 7;
        case '/': return 6;
        default: return 16;
    }
}

void Frequency::drawFrequency() {
    char cFreq[STR_BUFFER_SIZE] = "\0";
    char dFreq[STR_BUFFER_SIZE] = "\0";
    ultoa(this->state->frequency, cFreq, 10);
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
    this->display->tft->setTextSize(1);
    this->display->tft->setFont(&FreeSansBold15pt7b);

    int16_t xof = FREQUENCY_X + (this->state->frequency < 1E+7 ? 10 : 0);
    for (int8_t i = 0; i < STR_BUFFER_SIZE-1; i++) {
        auto osl = symbolLength(oldFreq[i]);
        auto nsl = symbolLength(dFreq[i]);
        if (oldFreq[i] != dFreq[i]) {
            if (osl != nsl) {
                // we have a shift
                Bounds t = {};
                this->display->tft->getTextBounds(oldFreq, FREQUENCY_X, FREQUENCY_Y, &t.x, &t.y, &t.w, &t.h);
                this->display->tft->fillRect(t.x, t.y, t.w, t.h, ST77XX_BLACK);
                break;
            } else {
                this->display->tft->fillRect(xof, FREQUENCY_Y-21, osl, 22, ST77XX_BLACK);
            }
        }
        xof += osl ;
    }

    this->display->textxy(
            FREQUENCY_X + (this->state->frequency < 1E+7 ? 10 : 0),
            FREQUENCY_Y,
            dFreq,
            COLOR_BRIGHT_GREEN,
            ST77XX_BLACK
    );

    strcpy(oldFreq, dFreq);
    this->display->tft->setFont();
    if (this->redrawType == Full) {
        this->display->textxy(0, STEP_Y, F("Frequency:"), COLOR_GRAY_MEDIUM, ST77XX_BLACK);
    }
}