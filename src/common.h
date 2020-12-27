//
// Created by Philipp Tkachev on 2018-10-26.
//

#ifndef NANODDS_COMMON_H
#define NANODDS_COMMON_H

#include "State.h"
#include "AppSettings.h"


char b[STR_BUFFER_SIZE];





volatile State state;
AppSettings settings;

// volatile keyword must be used for global variables used inside interrupt handlers
// @see https://barrgroup.com/Embedded-Systems/How-To/C-Volatile-Keyword

volatile uint32_t oFrequency = 0; // Hz

#endif //NANODDS_COMMON_H
