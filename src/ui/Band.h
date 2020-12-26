//
// Created by Philipp Tkachev on 2019-08-27.
//

#ifndef NANODDS_BAND_H
#define NANODDS_BAND_H

#include <Adafruit_MCP23017.h>
#include "../State.h"
#include "Display.h"
#include "Widget.h"
// #include "../common.h"

class Band: public Widget {
    private:
        volatile State *state;
    Adafruit_MCP23017 *mcp;
        volatile uint16_t scalePosX = 0;
        volatile uint32_t frequency = 0;

public:
        uint8_t total = BANDS;
        explicit Band(Display *display, volatile State *state, Adafruit_MCP23017 *mcp) {
            this->display = display;
            this->state = state;
            this->mcp = mcp;
            this->left = 0;
            this->top = 0;
            this->width = static_cast<uint8_t>(this->display->tft->width());
            this->height = 32;
        };
    void displayScale(bool redraw);
    void scaleTriangle(uint16_t c);
    void draw() override;
    void loop() override;
    void next();
    using Widget::scheduleRedraw;
    using Widget::setVisibility;

};

#endif //NANODDS_BAND_H
