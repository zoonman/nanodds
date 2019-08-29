//
// Created by Philipp Tkachev on 2019-08-27.
//

#include "Band.h"

void Band::draw() {
    if (!this->isVisible) {
        return;
    }
    this->isRedrawForced = false;

    if (state->band == BANDS) {
        this->display->drawRoundTextBox(
                TFT_WIDTH - TFT_QUOTER_WIDTH,
                0,
                TFT_QUOTER_WIDTH,
                15,
                MsgAir,
                COLOR_BRIGHT_RED,
                COLOR_DARK_RED
        );
        this->display->drawRoundTextBox(TFT_WIDTH - TFT_QUOTER_WIDTH, 0, TFT_QUOTER_WIDTH, 15, MsgAir, COLOR_BRIGHT_RED,
                                        COLOR_DARK_RED);
    } else {
        String b2(BandsBounds[state->band].id);
        this->display->drawRoundTextBox(TFT_WIDTH - TFT_QUOTER_WIDTH, 0, TFT_QUOTER_WIDTH, 15, &b2, COLOR_BRIGHT_GREEN,
                                        COLOR_DARK_GREEN);
    }
    this->display->tft->fillRect(0, SCALE_Y - 2, TFT_WIDTH - 1, SCALE_T * 2, ST77XX_BLACK);

    // because we are doing full scale redraw, we have to force this
    this->displayScale(true);
}

void Band::scaleTriangle(uint16_t c) {
    this->display->tft->fillTriangle(scalePosX, SCALE_Y - 1, scalePosX - SCALE_T, SCALE_Y - SCALE_T - 1, scalePosX + SCALE_T, SCALE_Y - 1 - SCALE_T, c);
}

void Band::displayScale(bool redraw) {
    uint16_t fStep = 500; // kHz
    uint16_t fStart = 1000; // kHz
    uint16_t fWidth = 30000; // kHz
    if (state->band != BANDS) {
        fStep = 50;
        fStart = BandsBounds[state->band].start;
        fWidth = BandsBounds[state->band].width;
    }
    auto newScalePosX = static_cast<uint16_t>((state->frequency/1000 - fStart) * TFT_WIDTH / fWidth);
    if (scalePosX != newScalePosX || redraw) {
        // draw boundaries
        this->display->tft->drawFastHLine(0, SCALE_Y, TFT_WIDTH-1, COLOR_GRAY_MEDIUM);
        this->display->tft->drawFastVLine(0, SCALE_Y - 2, 5, COLOR_GRAY_MEDIUM);
        this->display->tft->drawFastVLine(TFT_WIDTH-1, SCALE_Y - 2, 5, COLOR_GRAY_MEDIUM);
        // draw ticks
        for (uint32_t f = 0; f < fWidth; f += fStep) {
            this->display->tft->drawFastVLine(
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

void Band::loop() {

    // detecting qualified events for redrawing
    if (this->frequency != this->state->frequency) {

        // 1 band has changed
        uint8_t i;
        for (i = 0; i < total; i++) {
            if (this->state->frequency / 1000 >= BandsBounds[i].start &&
                this->state->frequency / 1000 <= BandsBounds[i].start + BandsBounds[i].width) {
                if (state->band != i) {
                    state->band = i;
                    this->isRedrawForced = true;
                }
                break;
            }
        }
        if (i == total && state->band != i) {
            state->band = i;
            this->isRedrawForced = true;
        }
        this->frequency = this->state->frequency;

        // this will call redraw only for scale portion
        this->displayScale(false);
    }

    if (this->isRedrawForced) {
        this->draw();
    }
}

void Band::next() {
    uint8_t newBand;
    if (this->state->band < total - 1) {
        newBand = this->state->band + (uint8_t) 1;
    } else {
        newBand = 0;
    }
    this->state->frequency =
            (static_cast<uint32_t>(BandsBounds[newBand].start + (BandsBounds[newBand].width / 2))) * 1000;
    this->state->band = newBand;
    this->isRedrawForced = true;
}

// idea! smart band mode:
// when we reach end of the band it jumps to the beggining of the next ones
/*
class Bands {
public:
    Bands() = default;

    const uint8_t total = BANDS;

    void render();

    void update();

    void next() {
        int8_t newBand;
        if (state.band < total - 1) {
            newBand = state.band + (uint8_t) 1;
        } else {
            newBand = 0;
        }
        state.frequency = (static_cast<uint32_t>(BandsBounds[newBand].start + (BandsBounds[newBand].width / 2))) * 1000;
    }

private:
    bool smart = false;
};

void Bands::render() {
    if (state.band == total) {
        drawRoundTextBox(TFT_WIDTH - TFT_QUOTER_WIDTH, 0, TFT_QUOTER_WIDTH, 15, "AIR", COLOR_BRIGHT_RED,
                         COLOR_DARK_RED);
    } else {
        itoa(BandsBounds[state.band].id, b, 10);
        drawRoundTextBox(TFT_WIDTH - TFT_QUOTER_WIDTH, 0, TFT_QUOTER_WIDTH, 15, b, COLOR_BRIGHT_GREEN,
                         COLOR_DARK_GREEN);
    }
    tft.fillRect(0, SCALE_Y - 2, TFT_WIDTH - 1, SCALE_T * 2, ST77XX_BLACK);
}

void Bands::update() {

}
*/