//
// Created by Philipp Tkachev on 2019-03-08.
//

#include <Arduino.h>

#ifndef NANODDS_ACTION_H
#define NANODDS_ACTION_H

#include "callback.h"

class Menu;

class Action {
    public:
        String name;
        bool checkable = false;
        bool isActive = false;
        bool isChecked = false;
        bool isEnabled = true;
        bool hasSubMenu = false;
        Menu* subMenu = nullptr;
        void render();
        void setSubmenu(Menu* menu);
        void setActive(bool status);
        void setCallback(CALLBACK);
        void trigger();
private:
    CALLBACK cb;
};

#endif //NANODDS_ACTION_H
