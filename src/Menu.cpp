//
// Created by Philipp Tkachev on 2019-03-08.
//

#include "Menu.h"

void Menu::addAction(Action *action) {
    actions[length++] = action;
}

void Menu::setActive(bool value) {
    this->active = value;
}

bool Menu::isActive() {
    return active;
}
// @todo: scrolling
void Menu::up() {
    if (current > 0) {
        actions[current]->setActive(false);
        current--;
        actions[current]->setActive(true);
    }
}

// @todo: scrolling
void Menu::down() {
    if (current < length - 1) {
        actions[current]->setActive(false);
        current++;
        actions[current]->setActive(true);
    }
}

