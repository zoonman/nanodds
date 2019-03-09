//
// Created by Philipp Tkachev on 2019-03-08.
//

#include "Menu.h"

void Menu::addAction(Action *action) {
    action->setIndex(this->length);
    action->setDisplay(this->display);
    this->actions[this->length++] = action;
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
    this->display->tft->fillScreen(ST77XX_BLACK);
    this->display->drawRoundTextBox(
            0,
            0,
            160,
            MENU_ITEM_HEIGHT,
            F("Menu"),
            ST7735_WHITE,
            COLOR_DARK_GRAY
    );
    // @todo scrolling
    if (this->length < MAX_MENU_ACTIONS_PER_SCREEN) {
        for (uint8_t i = 0; i < this->length; ++i) {
            this->actions[i]->setActive(this->selectedActionIndex == i);
            this->actions[i]->render();
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
void Menu::setCurrentMenu(Menu* ptcm) {
    this->currentMenu = ptcm;
}

void Menu::exit() {
    this->display->tft->fillScreen(ST77XX_BLACK);
    this->setActive(false);
    if (this->parentMenu == nullptr) {
        return;
    }
    *(this->currentMenu) = *(this->parentMenu);
    (this->currentMenu)->setActive(true);
}

