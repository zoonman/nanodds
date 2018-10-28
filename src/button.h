//
// Created by Philipp Tkachev on 2018-10-27.
//

#ifndef NANODDS_BUTTON_H
#define NANODDS_BUTTON_H

#include <Arduino.h>

class Button {
public:
    Button(uint8_t pin) {
        this->pin = pin;
    }
    void setup() {
        pinMode(pin, INPUT_PULLUP);
    }

    void loop() {
        // if !reset, track time
        int state = digitalRead(pin);
        auto ms = millis();
        if (state == LOW) {
            if (ms - pressStarted > debounce && ms - pressReleased < debounce) {

            }
        }
    }

    bool isShortPress() {

    }

    bool isLongPress() {

    }

    bool isPressed() {

    }
private:
    uint8_t pin;
    uint32_t timer;
    uint32_t pressStarted;
    uint32_t pressReleased;
    bool reset;
    const uint8_t debounce = 100;
};


#endif //NANODDS_BUTTON_H
