//
// Created by Philipp Tkachev on 2019-03-08.
//

#include <Arduino.h>
#include <Adafruit_ST7735.h>

#ifndef NANODDS_ACTION_H
#define NANODDS_ACTION_H

#include "../ui/Display.h"
#include "../callback.h"
#include "../Mode.h"

#define MENU_ITEM_HEIGHT (uint8_t)16

class Menu;

class Action {
public:
    explicit Action(Message m);

    // explicit Action(String *name);

    Message message = MsgMenu;

    uint8_t height = MENU_ITEM_HEIGHT;
    bool isActive = false;

    bool hasSubMenu();

    void render();

    void setSubMenu(Menu *menu);
    void setCurrentMenu(Menu** menu);

    void setActive(bool status);

    void setIndex(size_t index);

    void setCallback(CALLBACK);

    void select();

    void setDisplay(Display *display);

private:
    CALLBACK cb = nullptr;
    Menu *subMenu = nullptr;
    Menu **currentMenu = nullptr;
    Menu *parentMenu = nullptr;
    Display *display;
    size_t index = 0;
};

#endif //NANODDS_ACTION_H
