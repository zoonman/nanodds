//
// Created by Philipp Tkachev on 2018-10-25.
//

#ifndef NANODDS_SMETER_H
#define NANODDS_SMETER_H

#define BAR_WIDTH             (uint8_t)14

#include "../config.h"
#include "Widget.h"

class SMeter: Widget {
public:
    explicit SMeter(uint8_t pin, Display *display) {
        pd = 1;
        this->pin = pin;
        this->display = display;
    };
    void setup();
    void drawLevelBar(uint8_t l);
    void drawLevel(uint8_t l);
    void draw() final;
    uint16_t level2color(uint8_t l);
    void loop() override;
private:
    volatile uint8_t pl = 1;
    int pd = 0;
    uint8_t pin;
};

#endif //NANODDS_SMETER_H
