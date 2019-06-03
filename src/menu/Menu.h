//
// Created by Philipp Tkachev on 2018-12-25.
//

#include <Arduino.h>

#ifndef NANODDS_MENU_H
#define NANODDS_MENU_H

#define MAX_MENU_ACTIONS (size_t)7
#define MAX_MENU_ACTIONS_PER_SCREEN (ST7735_TFTHEIGHT_128 / MENU_ITEM_HEIGHT - 1)

#include "Action.h"
#include <Adafruit_ST7735.h>

class Menu {
public:
    explicit Menu(Display *display, Message *messages, size_t length) {
        this->display = display;
        this->messages = messages;
        this->length = length;
    }

    virtual void up();
    virtual void down();
    void render();
    void select();
    void exit();
    bool isActive();
    void setActive(bool value);
    void setDisplay(Display *display);
    void renderItem(size_t i);
    Message getActiveMessage();
private:
    Display *display;
    Message *messages;
    size_t length = 0;
    bool active = false;
    size_t selectedActionIndex = 0;
};


// Bool

// Integer

// TXT?

/**
 * Menu:
 *  Backlight ?
 *  SWR Analysis
 *  CW
 *      Mode:
 *          () Simple
 *          () Iambic A
 *          () Iambic B
 *      Speed, WPM: 1..255
 *      Tone frequency: 700 Hz
 *  HF Settings
 *      IF: 8000000 Hz
 *      SSB Offset: 2400 Hz
 *      Correction: 0 Hz
 *      Power calibration
 *      SWR Calibration
 *  About
 *  Reset
 *
 *
 */
/*
enum MenuItemType {INT, STRING, CHECKBOX, COMBOBOX, FOLDER};

struct MenuItem {
    String name;
    bool isActive;
    MenuItemType type;
};

struct Menu {
    MenuItem items[2] = {
            {F("Memory"), false, FOLDER},
            {F("CW"), false, FOLDER},
    };
} mainMenu;
*/

#endif //NANODDS_MENU_H
