#include <Arduino.h>


#ifndef NANODDS_CONFIG_H
#define NANODDS_CONFIG_H

// Frequencies

#define UPPER_RX_BOUND      30000000
#define LOWER_RX_BOUND      500000

#define START_FREQUENCY     (uint32_t)1199000
#define INTERMEDIATE_FREQUENCY 7997000

// Pins
/*
// http://wiki.microduinoinc.com/Microduino-Module_Core%2B
// Use "Map Pin Name" column for semantic names

// #define TX_BTN_PIN          6 // (INT2/AIN0)PB2
// #define TX_BTN_PIN          D6 // (INT2/AIN0)PB2
#define TX_BTN_PIN          PB2 // (INT2/AIN0)PB2
// #define MEM_BTN_PIN         16 // (TDI)PC5
// #define MEM_BTN_PIN         D16 // (TDI)PC5
#define MEM_BTN_PIN         PC5 // (TDI)PC5
//#define MODE_BTN_PIN        17 // (TDO)PC4
// #define MODE_BTN_PIN        D17 // (TDO)PC4
#define MODE_BTN_PIN        PC4 // (TDO)PC4
// #define VFO_BTN_PIN         18 // (TMS)PC3
// #define VFO_BTN_PIN         D18 // (TMS)PC3
#define VFO_BTN_PIN         PC3 // (TMS)PC3
// #define STEP_BTN_PIN        26 // (ADC5)PA5
// #define STEP_BTN_PIN        A2 // (ADC5)PA5
#define STEP_BTN_PIN        PA5 // (ADC5)PA5
// #define BAND_BTN_PIN        27 // (ADC4)PA4
// #define BAND_BTN_PIN        A3 // (ADC4)PA4
#define BAND_BTN_PIN        PA4 // (ADC4)PA4
// #define BACKLIGHT_PIN       9  // (OC1A)PD5
// #define BACKLIGHT_PIN       D9  // (OC1A)PD5
#define BACKLIGHT_PIN       PD5  // (OC1A)PD5
// #define TFT_BACKLIGHT_PIN   7  // (OC0A/AIN1)PB3
// #define TFT_BACKLIGHT_PIN   D7  // (OC0A/AIN1)PB3
#define TFT_BACKLIGHT_PIN   PB3  // (OC0A/AIN1)PB3

// #define SMETER_INPUT_PIN    31 // PA0, D31 or A7
// #define SMETER_INPUT_PIN    31 // PA0, D31 or A7
#define SMETER_INPUT_PIN    PA0 // PA0, D31 or A7
#define PANO_INPUT_PIN      PA2 // PA2, D29 or A5

#define SWR_REF_INPUT_PIN   PA0 // PA0, D31 or A7
#define SWR_FOR_INPUT_PIN   PA1 // PA1, D30 or A6

// encoder
// #define ENCODER_LEFT_PIN    D2 //
#define ENCODER_LEFT_PIN    PD2 //
// #define ENCODER_RIGHT_PIN   D3
#define ENCODER_RIGHT_PIN   PD3
// #define ENCODER_PUSH_PIN    22 // 22
#define ENCODER_PUSH_PIN    PD4 // 22

// display
// #define TFT_CS                10 // D10, Chip select pin #44, SS, PB4 pinout http://wiki.microduinoinc.com/Microduino-Module_Core%2B
// #define TFT_CS                D10 // D10, Chip select pin #44, SS, PB4 pinout http://wiki.microduinoinc.com/Microduino-Module_Core%2B
#define TFT_CS                PB4 // D10, Chip select pin #44, SS, PB4 pinout http://wiki.microduinoinc.com/Microduino-Module_Core%2B
// #define TFT_RST                4  // D4, Reset pin #40 (optional, pass -1 if unused), PB0
//#define TFT_RST               D4  // D4, Reset pin #40 (optional, pass -1 if unused), PB0
#define TFT_RST               PB0  // D4, Reset pin #40 (optional, pass -1 if unused), PB0
// #define TFT_DC                 5  // A0, Data/Command pin #41 (PB1)
// #define TFT_DC                D5  // A0, Data/Command pin #41 (PB1)
#define TFT_DC                PB1  // A0, Data/Command pin #41 (PB1)

// Special flags

// #define ENCODER_OPTIMIZE_INTERRUPTS
// #define TWI_FREQ            400000L

// #define USE_FAST_PINIO
//#define SPI_DEFAULT_FREQ 4000000
*/
#define TX_BTN_PIN          2 // (INT2/AIN0)PB2
#define MEM_BTN_PIN         21 // (TDI)PC5
#define MODE_BTN_PIN        20 // (TDO)PC4
#define VFO_BTN_PIN         19 // (TMS)PC3
#define STEP_BTN_PIN        29 // (ADC5)PA5
#define BAND_BTN_PIN        28 // (ADC4)PA4
#define BACKLIGHT_PIN       13 //13  // (OC1A)PD5
#define TFT_BACKLIGHT_PIN   3  // PB3  // (OC0A/AIN1)PB3
#define SMETER_INPUT_PIN    A0 // 24 // PA0, D31 or A7
#define PANO_INPUT_PIN      A2 //  26 // PA2, D29 or A5
#define SWR_REF_INPUT_PIN   A0 // PA0, D31 or A7
#define SWR_FOR_INPUT_PIN   A1 // PA1, D30 or A6
#define ENCODER_LEFT_PIN    10 // PD2
#define ENCODER_RIGHT_PIN   11 // PD3
#define ENCODER_PUSH_PIN    12 // PD4

#define TFT_CS                4 // PB4 // D10, Chip select pin #44, SS, PB4 pinout http://wiki.microduinoinc.com/Microduino-Module_Core%2B
#define TFT_RST               0 // PB0  // D4, Reset pin #40 (optional, pass -1 if unused), PB0
#define TFT_DC                1 // PB1  // A0, Data/Command pin #41 (PB1)


// display
#define TFT_HEIGHT            (uint8_t)ST7735_TFTHEIGHT_128
#define TFT_WIDTH             (uint8_t)160
#define TFT_QUOTER_WIDTH      (uint8_t)(TFT_WIDTH/4)
#define GRID                  (uint8_t)(TFT_HEIGHT/16) // 8
#define SCALE_Y               (uint8_t)(GRID*10)
#define PANO_Y                (uint8_t)(GRID*11 - 3)
#define STEP_Y                (uint8_t)(GRID*3)
#define RIT_Y                 (uint8_t)(GRID*8)
#define SWR_SCALE_Y           (uint8_t)(GRID*10)
#define SWR_SCALE_TY          (uint8_t)(SWR_SCALE_Y - GRID)

#define BANDS                 8

#define SCALE_T               (uint8_t)5

#define ENCODER_DO_NOT_USE_INTERRUPTS 1

// Bands
struct BandRecord {
    uint8_t id; // meters
    uint16_t start; // kHZ
    uint16_t width; // kHz
};

#pragma pack(push, 1)
const static BandRecord BandsBounds[BANDS] = {
        {160, 1800,  200},
        {80,  3500,  500},
        {40,  7000,  300},
        {30,  10100, 150},
        {20,  14000, 350},
        {17,  18068, 100},
        {15,  21000, 450},
        {10,  28000, 1700}
};
#pragma pack(pop)

#endif //NANODDS_CONFIG_H
