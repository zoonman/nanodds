#ifndef NANODDS_CONFIG_H
#define NANODDS_CONFIG_H

// Frequencies

#define UPPER_RX_BOUND      30000000
#define LOWER_RX_BOUND      500000

#define START_FREQUENCY     (uint32_t)1199000
#define INTERMEDIATE_FREQUENCY 7895000

// Pins

// http://wiki.microduinoinc.com/Microduino-Module_Core%2B
#define TX_BTN_PIN          6 // (INT2/AIN0)PB2
#define MEM_BTN_PIN         16
#define MODE_BTN_PIN        17
#define VFO_BTN_PIN         18
#define STEP_BTN_PIN        26
#define BAND_BTN_PIN        27
#define BACKLIGHT_PIN       9
#define TFT_BACKLIGHT_PIN   7

#define SMETER_INPUT_PIN    31
#define PANO_INPUT_PIN      29 // PA2, D29 or A5

#define SWR_REF_INPUT_PIN   31
#define SWR_FOR_INPUT_PIN   31

// encoder
#define ENCODER_LEFT_PIN    2
#define ENCODER_RIGHT_PIN   3
#define ENCODER_PUSH_PIN    22

// display
#define TFT_CS                10 // D10, Chip select pin #44, SS, PB4 pinout http://wiki.microduinoinc.com/Microduino-Module_Core%2B
#define TFT_RST                4  // D4, Reset pin #40 (optional, pass -1 if unused), PB0
#define TFT_DC                 5  // A0, Data/Command pin #41 (PB1)

// Special flags

#define ENCODER_OPTIMIZE_INTERRUPTS
#define TWI_FREQ            400000L

#define USE_FAST_PINIO
//#define SPI_DEFAULT_FREQ 4000000

#endif //NANODDS_CONFIG_H
