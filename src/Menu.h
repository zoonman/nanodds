//
// Created by Philipp Tkachev on 2018-12-25.
//

#include <Arduino.h>

#ifndef NANODDS_MENU_H
#define NANODDS_MENU_H

#define MAX_MENU_ACTIONS 10

#include "Action.h"

class Action;

class Menu {
public:
    virtual void up();
    virtual void down();
    virtual void addAction(Action *action);
    bool isActive();
    void setActive(bool value);
private:
    Action* actions[MAX_MENU_ACTIONS];
    size_t length = 0;
    bool active = false;
    size_t current = 0;
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
} menu;
*/

#endif //NANODDS_MENU_H
