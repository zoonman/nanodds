//
// Created by Philipp Tkachev on 2018-10-27.
//

#ifndef NANODDS_BUTTON_H
#define NANODDS_BUTTON_H

typedef void (*CALLBACK) (void);

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
        int currentPinState = digitalRead(pin);
        auto ms = millis();
        auto tdf = ms - this->states[currentPinState].started;

        if (this->states[currentPinState].isActive) {
            if (tdf > this->debounceThreshold) { // we have a "settled" state
                if (tdf > this->longPressThreshold) {
                    if (this->longPressCallback != nullptr && currentPinState == this->pressedState) {
                        this->longPressCallback();
                        this->states[currentPinState].stableStarted = this->states[currentPinState].started = ms;
                        this->states[this->pressedState].stableStarted = this->states[this->releasedState].stableStarted = ms;
                    }
                } else {
                    //
                    auto rpdft = this->states[this->releasedState].stableStarted - this->states[this->pressedState].stableStarted;
                    if (currentPinState == this->releasedState && rpdft > this->shortPressThreshold && rpdft < this->longPressThreshold) {
                        if (this->shortPressCallback != nullptr) {
                            this->shortPressCallback();
                            this->states[this->pressedState].stableStarted = this->states[this->releasedState].stableStarted = ms;
                        }
                    }
                }
                this->states[currentPinState].stableStarted = this->states[currentPinState].started;
            }
        } else {
            this->states[currentPinState].started = ms;
        }
        this->states[this->pressedState].isActive = (currentPinState == this->pressedState);
        this->states[this->releasedState].isActive = (currentPinState == this->releasedState);
    }

    bool isPressed() {
        return this->states[this->pressedState].isActive;
    }

    bool isReleased() {
        return this->states[this->releasedState].isActive;
    }

    void registerShortPressCallback(CALLBACK cb) {
        this->shortPressCallback = cb;
    }

    void registerLongPressCallback(CALLBACK cb) {
        this->longPressCallback = cb;
    }

private:
    uint8_t pin;

    const uint16_t longPressThreshold = 1000; // ms
    const uint16_t shortPressThreshold = 100; // ms
    const uint16_t debounceThreshold = 10; // ms

    struct State {
        bool isActive = false;
        //bool isLongPress = false;
        //bool isShortPress = false;
        unsigned long started = 0;
        unsigned long stableStarted = 0;
    };
    int pressedState = LOW;
    int releasedState = HIGH;

    CALLBACK shortPressCallback = nullptr;
    CALLBACK longPressCallback = nullptr;

    State states[2];
};


#endif //NANODDS_BUTTON_H
