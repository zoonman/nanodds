//
// Created by Philipp Tkachev on 2019-03-08.
//

#include "Action.h"

void Action::setActive(bool status) {
    this->isActive = status;
    this->render();
}

void Action::setSubmenu(Menu *menu) {
    subMenu = menu;
}

void Action::render() {
    // @todo

}

void Action::setCallback(CALLBACK callback) {
    this->cb = callback;
}

void Action::trigger() {
    if (this->cb == nullptr)
        return;
    this->cb();
}