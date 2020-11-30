//
// Created by Philipp Tkachev on 2018-10-26.
//

#ifndef NANODDS_COMMON_H
#define NANODDS_COMMON_H

#include "State.h"
#include "AppSettings.h"

#define STR_BUFFER_SIZE       12
char b[STR_BUFFER_SIZE];


// positions
#define FREQUENCY_X           (uint8_t)10
#define FREQUENCY_Y           (uint8_t)56
#define FREQUENCY_FAKE_SPACE  '/'



volatile State state;
AppSettings settings;

// volatile keyword must be used for global variables used inside interrupt handlers
// @see https://barrgroup.com/Embedded-Systems/How-To/C-Volatile-Keyword

volatile uint32_t oFrequency = 0; // Hz

#endif //NANODDS_COMMON_H
