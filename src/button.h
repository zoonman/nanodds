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
        int pinState = digitalRead(pin);
        auto ms = millis();
        if (states[pinState].isActive) {
            if (ms - states[pinState].started > longPressThreshold) {
                states[pinState].isLongPress = true;
            } else if (ms - states[pinState].started > shortPressThreshold) {
                states[pinState].isShortPress = true;
            } else {
                states[pinState].isLongPress = false;
                states[pinState].isShortPress = false;
            }
        } else {
            states[pinState].isActive = true;
            states[pinState].started = ms;
            states[pinState].isLongPress = false;
            states[pinState].isShortPress = false;
        }

        lastState = pinState;
    }

    bool isShortPress() {
        bool stateBefore = states[pressedState].isShortPress;
        if (stateBefore) {
            states[pressedState].isShortPress = false;
            resetState(pressedState);
        }
        return stateBefore;
    }

    bool isLongPress() {
        bool stateBefore = states[pressedState].isLongPress;
        if (stateBefore) {
            states[pressedState].isLongPress = false;
            resetState(pressedState);
        }
        return stateBefore;
    }

    bool isPressed() {
        return states[pressedState].isShortPress;
    }

    void resetState(int bState) {
        states[bState].started = millis();
    }
private:
    uint8_t pin;

    const uint16_t longPressThreshold = 1000; // ms
    const uint16_t shortPressThreshold = 100; // ms

    struct State {
        bool isActive = false;
        bool isLongPress = false;
        bool isShortPress = false;
        unsigned long started = 0;
    };
    int lastState = HIGH;
    int pressedState = LOW;

    State states[2];
};


#endif //NANODDS_BUTTON_H
