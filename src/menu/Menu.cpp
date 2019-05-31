//
// Created by Philipp Tkachev on 2019-03-08.
//
#include <stdlib.h>
#include "Menu.h"

void Menu::addAction(Action *action) {
    action->setIndex(this->length);
    action->setDisplay(this->display);
    size_t nl = ++this->length;
    // this->actions = (Action * *)realloc(this->actions, nl * sizeof(*action));
    this->actions[this->length-1] = action;
}

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
        this->actions[this->selectedActionIndex]->setActive(false);
        this->selectedActionIndex--;
        this->actions[this->selectedActionIndex]->setActive(true);
    }
}

// @todo: scrolling
void Menu::down() {
    if (this->selectedActionIndex < this->length - 1) {
        this->actions[this->selectedActionIndex]->setActive(false);
        this->selectedActionIndex++;
        this->actions[this->selectedActionIndex]->setActive(true);
    }
}

void Menu::render() {
    this->display->clear();
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
    // @todo scrolling
    //if (this->length < MAX_MENU_ACTIONS_PER_SCREEN)
    {
        for (size_t i = 0; i < this->length; i++) {
            this->actions[i]->setActive(this->selectedActionIndex == i);
            // this->actions[i]->render();
        }
    }
}

void Menu::select() {
    this->actions[this->selectedActionIndex]->select();
}

void Menu::setParentMenu(Menu *pm) {
    this->parentMenu = pm;
}

/**
 * We change pointer to a pointer
 * @param pm
 */
void Menu::setCurrentMenu(Menu** ptcm) {
    this->currentMenu = ptcm;
}

void Menu::exit() {
    this->display->clear();
    this->setActive(false);
    if (this->parentMenu == nullptr) {
        return;
    }
    *this->currentMenu = this->parentMenu;
    (*(*this->currentMenu)).setActive(true);
    (*(*this->currentMenu)).render();
}

