//
// Created by Philipp Tkachev on 2018-10-27.
//

#ifndef NANODDS_BUTTON_H
#define NANODDS_BUTTON_H

#include <Arduino.h>
class Button {
public:
    explicit Button(uint8_t pin) {
        this->pin = pin;
    }

    void setup() {
        pinMode(pin, INPUT_PULLUP);
    }
    /**
     *
     */
    void loop() {
        // if !reset, track time
        int pinState = digitalRead(pin);
        auto ms = millis();
        auto tdf = ms - this->states[pinState].started;

        if (this->states[pinState].isActive) {
            if (tdf > this->debounceThreshold) { // we have a "settled" state
                if (tdf > this->longPressThreshold) {
                    this->states[this->lastState].isLongPress = true;
                    this->states[this->lastState].isShortPress = false;
                } else {
                    //
                    auto rpdft = this->states[this->releasedState].stableStarted - this->states[this->pressedState].stableStarted;
                    if (pinState == this->releasedState && rpdft > this->shortPressThreshold && rpdft < this->longPressThreshold) {
                        this->states[this->pressedState].isShortPress = !this->states[this->pressedState].isLongPress;
                    } else {
                        this->states[this->pressedState].isShortPress = false;
                    }

                }
                this->lastState = pinState;

                this->states[pinState].stableStarted = this->states[pinState].started;
            }
        } else {
            this->states[pinState].started = ms;
            this->lastState = pinState;
        }
        this->states[this->pressedState].isActive = (pinState == LOW);
        this->states[this->releasedState].isActive = (pinState == HIGH);

    }

    bool isShortPress() {
        bool stateBefore = this->states[this->pressedState].isShortPress;
        if (stateBefore) {
            this->states[this->pressedState].isShortPress = false;
            this->resetState(this->pressedState);
        }
        return stateBefore;
    }

    bool isLongPress() {
        bool stateBefore = this->states[this->pressedState].isLongPress;
        if (stateBefore) {
            this->states[this->pressedState].isLongPress = false;
            // this->resetState(this->pressedState);
        }
        return stateBefore;
    }

    bool isPressed() {
        return this->states[this->pressedState].isActive;
    }

    bool isReleased() {
        return this->states[this->releasedState].isActive;
    }

    void resetState(int bState) {
        this->states[bState].started = millis();
        this->states[bState].stableStarted = millis();
    }
private:
    uint8_t pin;

    const uint16_t longPressThreshold = 1000; // ms
    const uint16_t shortPressThreshold = 50; // ms
    const uint16_t debounceThreshold = 10; // ms

    struct State {
        bool isActive = false;
        bool isLongPress = false;
        bool isShortPress = false;
        unsigned long started = 0;
        unsigned long stableStarted = 0;
    };
    int lastState = HIGH;
    int pressedState = LOW;
    int releasedState = HIGH;

    State states[2];
};


#endif //NANODDS_BUTTON_H
