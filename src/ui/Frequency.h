//
// Created by Philipp Tkachev on 2019-10-29.
//

#ifndef NANODDS_FREQUENCY_H
#define NANODDS_FREQUENCY_H

#include <si5351.h>
#include "../State.h"
#include "Display.h"
#include "Widget.h"
#include "FreeSansBold15pt7b.h"

// positions
#define FREQUENCY_X           (uint8_t)10
#define FREQUENCY_Y           (uint8_t)56
#define FREQUENCY_FAKE_SPACE  '/'

class Frequency: public Widget {
private:
    uint64_t cachedStateFrequency = 1;

    volatile State *state;
    AppSettings *settings;
    Si5351 *pll;
public:
    explicit Frequency(
            AppSettings *settings,
            Si5351 *pll,
            volatile State *state,
            Display *display) {
        this->state = state;
        this->pll = pll;
        this->display = display;
        this->settings = settings;
    };
    void draw() override;
    void loop() override;
    using Widget::scheduleRedraw;
    using Widget::setVisibility;
    void updatePllFrequency();
    void drawFrequency();
    void drawStep();
    void drawRIT();
    void changeFrequencyStep(int8_t offset);
};

#endif //NANODDS_FREQUENCY_H
