//
// Created by Philipp Tkachev on 2019-05-20.
//

#ifndef NANODDS_SPINNER_H
#define NANODDS_SPINNER_H

#include "Display.h"
#include "Widget.h"

template <typename T>
class Spinner: public Widget {
private:
    Message label;
    uint16_t color;
    boolean isFocused = false;
    boolean isRedraw = false;
    T value = 0;
    T step = 1;


public:
    explicit Spinner<T>(Display *display);

    void setValue(T value);

    void inc();

    void dec();

    void changeStep();

    uint8_t lg();

    T getValue();

    void setLabel(Message label) {
        this->label = label;
    };

    void setFocus(boolean focused) {
        this->isFocused = focused;
    }

    using Widget::scheduleRedraw;
    using Widget::setVisibility;


    void draw() override;
    void loop() override;
};

#endif //NANODDS_SPINNER_H
