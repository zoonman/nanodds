//
// Created by Philipp Tkachev on 2019-03-08.
//
#include <stdlib.h>
#include "Menu.h"

/*
void Menu::addAction(Action action) {
    this->length++;
    //this->actions[this->length-1] = action;
}
*/
void Menu::setActive(bool value) {
    if (!this->active && value) {
        this->render();
    }
    this->active = value;
}

void Menu::setDisplay(Display *display) {
    this->display = display;
};

bool Menu::isActive() {
    return this->active;
}
// @todo: scrolling
void Menu::up() {
    if (this->selectedActionIndex > 0) {
        this->selectedActionIndex--;
        this->renderItem(this->selectedActionIndex+1);
        this->renderItem(this->selectedActionIndex);
    }
}

// @todo: scrolling
void Menu::down() {
    if (this->selectedActionIndex < this->length - 1) {
        this->selectedActionIndex++;
        this->renderItem(this->selectedActionIndex-1);
        this->renderItem(this->selectedActionIndex);
    }
}
void Menu::renderItem(size_t i) {
    auto isActive = this->selectedActionIndex == i;
    this->display->drawRoundTextBox(
            MENU_ITEM_HEIGHT / 2,
            static_cast<uint8_t>(i * MENU_ITEM_HEIGHT) + MENU_ITEM_HEIGHT,
            static_cast<uint8_t>(this->display->tft->width() - MENU_ITEM_HEIGHT),
            MENU_ITEM_HEIGHT,
            this->messages[i],
            isActive ? COLOR_BRIGHT_GREEN : COLOR_GRAY_MEDIUM,
            isActive ? COLOR_DARK_GREEN : ST77XX_BLACK
    );
}

void Menu::render() {
    this->display->clear();
    this->display->tft->setTextSize(1);
    yield();
    this->display->drawRoundTextBox(
            0,
            0,
            this->display->tft->width(),
            MENU_ITEM_HEIGHT,
            MsgMenu,
            ST77XX_WHITE,
            ST77XX_BLUE
    );
    for (size_t i = 0; i < this->length; i++) {
        this->renderItem(i);
    }
}
Message Menu::getActiveMessage() {
    return this->messages[this->selectedActionIndex];
}
void Menu::select() {
    //this->actions[this->selectedActionIndex].cb();
}

void Menu::exit() {
    this->display->clear();
    this->setActive(false);
}

