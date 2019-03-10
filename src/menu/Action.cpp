//
// Created by Philipp Tkachev on 2019-03-08.
//

#include "Action.h"
#include "Menu.h"

void Action::setActive(bool status) {
    this->isActive = status;
    this->render();
}

void Action::setSubMenu(Menu *menu) {
    subMenu = menu;
}

void Action::setCurrentMenu(Menu** menu) {
    currentMenu = menu;
}

void Action::setIndex(size_t menuIndex) {
    this->index = menuIndex;
}

bool Action::hasSubMenu() {
    return this->subMenu != nullptr;
}

void Action::setCallback(CALLBACK callback) {
    this->cb = callback;
}

void Action::select() {
    if (this->hasSubMenu()) {
        this->parentMenu = *this->currentMenu;
        *this->currentMenu = this->subMenu;
        this->subMenu->setActive(true);
        // this->currentMenu->render();
    }
    if (this->cb == nullptr)
        return;
    this->cb();
}

void Action::render() {
    this->display->tft->setTextSize(1);
    if (this->isString) {
        this->display->drawRoundTextBox(
                0,
                static_cast<uint8_t>(this->index * this->height) + this->height,
                150,
                this->height,
                this->sname,
                this->isActive ? COLOR_BRIGHT_GREEN : COLOR_GRAY_MEDIUM,
                this->isActive ? COLOR_DARK_GREEN : ST77XX_BLACK
        );
    } else {
        this->display->drawRoundTextBox(
            0,
            static_cast<uint8_t>(this->index * this->height) + this->height,
            150,
            this->height,
            this->name,
            this->isActive ? COLOR_BRIGHT_GREEN : COLOR_GRAY_MEDIUM,
            this->isActive ? COLOR_DARK_GREEN : ST77XX_BLACK
        );
    }
}

void Action::setDisplay(Display *display) {
    this->display = display;
}

Action::Action(const __FlashStringHelper *name) : name(name) {this->isString = false;};
Action::Action(String *name) {
    this->isString = true;
    this->sname = name;
};