//
// Created by Philipp Tkachev on 2019-05-20.
//

#ifndef NANODDS_SPINNER_H
#define NANODDS_SPINNER_H

#include "Display.h"
#include "Widget.h"


class Spinner: public Widget {
private:
    String *label;
    uint16_t color;
    boolean isFocused;
    boolean isRedraw = false;
    int value;
    int step;


public:
    explicit Spinner(Display *display);

    void setValue(int value);

    void inc();

    void dec();

    int getValue();

    void setLabel(String *label) {
        this->label = label;
    };

    void setFocus(boolean focused) {
        this->isFocused = focused;
    }


    void draw() override;
    void loop() override;
};

#endif //NANODDS_SPINNER_H
