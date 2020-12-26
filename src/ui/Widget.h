//
// Created by Philipp Tkachev on 2019-05-20.
//

#ifndef NANODDS_WIDGET_H
#define NANODDS_WIDGET_H

#include "Display.h"
#include "../Task.h"

enum RedrawType {
    No,
    Data,
    Full
};

class Widget: public Task {
protected:
    Display *display{};
    uint8_t left{0};
    uint8_t top{0};
    uint8_t width{0};
    uint8_t height{0};
    boolean isVisible{false};
    RedrawType redrawType{No};
    virtual void draw() = 0;
public:
    void setVisibility(boolean isVisible);

    uint8_t getLeft() const;

    void setLeft(uint8_t left);

    uint8_t getTop() const;

    void setTop(uint8_t top);

    uint8_t getWidth() const;

    void setWidth(uint8_t width);

    uint8_t getHeight() const;

    void setHeight(uint8_t height);

    void scheduleRedraw(RedrawType type = Full);
};

#endif //NANODDS_WIDGET_H
