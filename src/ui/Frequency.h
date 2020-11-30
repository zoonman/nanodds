//
// Created by Philipp Tkachev on 2019-10-29.
//

#ifndef NANODDS_FREQUENCY_H
#define NANODDS_FREQUENCY_H

#include <si5351.h>
#include "../State.h"
#include "Display.h"
#include "Widget.h"

class Frequency: public Widget {
private:
    uint64_t semif = 1;
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
};

#endif //NANODDS_FREQUENCY_H
