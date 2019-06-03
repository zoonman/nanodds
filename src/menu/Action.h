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

struct Action {
    Action(Message msg, void (*cb)()) : msg(msg), cb(cb) {}

    Message msg;
    CALLBACK cb = nullptr;
};

#endif //NANODDS_ACTION_H
