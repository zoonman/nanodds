//
// Created by Philipp Tkachev on 2019-05-20.
//

#include "Widget.h"

void Widget::setVisibility(boolean isVisible) {
    if (this->isVisible != isVisible) {
        this->isVisible = isVisible;
        if (isVisible && !this->redrawType) {
            this->redrawType = Full;
        }
    }
}

uint8_t Widget::getLeft() const {
    return left;
}

void Widget::setLeft(uint8_t left) {
    Widget::left = left;
}

uint8_t Widget::getTop() const {
    return top;
}

void Widget::setTop(uint8_t top) {
    Widget::top = top;
}

uint8_t Widget::getWidth() const {
    return width;
}

void Widget::setWidth(uint8_t width) {
    Widget::width = width;
}

uint8_t Widget::getHeight() const {
    return height;
}

void Widget::setHeight(uint8_t height) {
    Widget::height = height;
}


void Widget::scheduleRedraw(RedrawType type) {
    this->redrawType = type;
};
