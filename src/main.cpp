#define ENCODER_OPTIMIZE_INTERRUPTS
#define TWI_FREQ 400000L
#define USE_FAST_PINIO

#define TX_BTN_PIN     6
#define MEM_BTN_PIN   16
#define MODE_BTN_PIN  17
#define VFO_BTN_PIN   18
#define STEP_BTN_PIN  26
#define BAND_BTN_PIN  27
#define BACKLIGHT_PIN  9

#define SMETER_INPUT_PIN 31
#define PANO_INPUT_PIN 5

#define SWR_REF_INPUT_PIN 31
#define SWR_FOR_INPUT_PIN 31


#define ENCODER_LEFT_PIN   2
#define ENCODER_RIGHT_PIN  3
#define ENCODER_PUSH_PIN  22

#include <Wire.h>


#include <Encoder.h>
#include <EEPROM.h>

#include <si5351.h>

#include "smeter.h"
#include "bands.h"
#include "button.h"
#include "pano.h"
#include "swr.h"
#include "Action.h"
#include "Menu.h"

#ifndef __FLASH
#warning __FLASH not defined here

#define PSTR_INIT( NAME, CONST_STR ) \
namespace _pstr_shadow_vars { static const char NAME[] PROGMEM { CONST_STR }; } \
constexpr PGM_P NAME { &_pstr_shadow_vars::NAME[0] };

#define AS_FLASH_STRING_HELPER( P )  (reinterpret_cast<const __FlashStringHelper * >(P))


#endif

volatile long encoderPosition  = 0;

//
Encoder freqEncoder(ENCODER_LEFT_PIN, ENCODER_RIGHT_PIN); // pin (2 = D2, 3 = D3)
Button freqEncButton(ENCODER_PUSH_PIN);
Si5351 si5351;

Bands bands;

SMeter sMeter(SMETER_INPUT_PIN);
Button txButton(TX_BTN_PIN);
Button memButton(MEM_BTN_PIN);   // PC3
Button modeButton(MODE_BTN_PIN); // PC4
Button vfoButton(VFO_BTN_PIN);   // PC5
Button stepButton(STEP_BTN_PIN); // PA5
Button bandButton(BAND_BTN_PIN); // PA4

Pano pano(PANO_INPUT_PIN);
SWRMeter swrMeter(SWR_REF_INPUT_PIN, SWR_FOR_INPUT_PIN);

Menu menu;

void setFrequency() {
    oFrequency = state.frequency;

    si5351.set_freq(
        static_cast<uint64_t>(state.frequency + INTERMEDIATE_FREQUENCY + (state.isRIT && !state.tx ? state.RITFrequency : 0))*100ULL,
        SI5351_CLK0
    );

    displayFrequency();
    bands.update();
    displayScale(false);
}

void render() {
    tft.fillScreen(ST77XX_BLACK);
    displayModulation();
    displayMode();
    bands.render();
    changeFrequencyStep(-1);
    displayScale(true);
    displayFrequency();
    sMeter.setup();
    displayRIT();
    displayVFO();
    //displaySWR();
}

long readVcc() {
    long result;
    // Read 1.1V reference against AVcc
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(2); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Convert
    while (bit_is_set(ADCSRA,ADSC));
    result = ADCL;
    result |= ADCH<<8;
    result = 1126400L / result; // Back-calculate AVcc in mV
    return result;
}


/*
TX - must be separate, external 1k
BAND short, MENU long
MODE
RIT

*/
unsigned long bs;

void switchBand() {
    bands.next();
    displayScale(true);
}


void switchRIT() {
    state.isRIT = !state.isRIT;
    displayRIT();
    setFrequency();
}

void switchBacklight() {
    int led = digitalRead(BACKLIGHT_PIN);
    if (led == LOW) {
        led = HIGH;
    } else {
        led = LOW;
    }
    digitalWrite(BACKLIGHT_PIN, static_cast<uint8_t>(led));
}

void enableCW() {
    state.mode = CW;
    displayWPM();
}
void enableLSB() {
    state.mode = LSB;
}
void enableUSB() {
    state.mode = USB;
}
void enableAM() {
    state.mode = AM;
}
void enableFM() {
    state.mode = FM;
}

void switchModulation() {
    switch (state.mode) {
        case CW:
            enableLSB();
            break;
        case LSB:
            enableUSB();
            break;
        case USB:
            enableAM();
            break;
        case AM:
            enableFM();
            break;
        case FM:
            enableCW();
            break;
    }
    displayModulation();
}

void switchVFO() {
    state.isAltFrequency = !state.isAltFrequency;
    uint32_t t = state.frequency;
    state.frequency = state.altFrequency;
    state.altFrequency = t;
    displayVFO();
    setFrequency();
}


void switchStep() {
    changeFrequencyStep(1);
}

auto loopMS = millis();

void buildMenu() {
    Action exitAction;
    exitAction.name = F("<< Exit");

    Action memoryAction;
    memoryAction.name = F("Memory");
    Menu memoryMenu;
    Action saveToNewCell;
    saveToNewCell.name =F("Save to a new cell");

    memoryMenu.addAction(&saveToNewCell);
    Action eraseCell;
    eraseCell.name =F("Erase data");
    memoryMenu.addAction(&eraseCell);


    memoryAction.setSubmenu(&memoryMenu);

    menu.addAction(&memoryAction);
    menu.addAction(&exitAction);

}

// SETUP -----------------------------------------------------------------------
void setup() {

    pinMode(BACKLIGHT_PIN, OUTPUT);

    //PORTD
    pinMode(7, OUTPUT);
    digitalWrite(7, 1);

    tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

    // put your setup code here, to run once:
    tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(1);
    //
    Serial.begin(9600);
    Serial.println("TWBR");
    Serial.println(TWBR);
    Serial.println(TWSR);
    // TWBR
    //Wire.


    si5351.init(SI5351_CRYSTAL_LOAD_0PF, 0, 0);
    Wire.setClock(400000);
    Serial.println("--");
    Serial.println(TWBR);
    Serial.println(TWSR);
    si5351.output_enable(SI5351_CLK0, 1);
    si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_4MA);
/*
       si5351.output_enable(SI5351_CLK2, 1);
    si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_4MA);
  */

    state.frequency = START_F;
    state.altFrequency = START_F + 1000;

    setFrequency();
    freqEncoder.write(encoderPosition);

    setFrequency();

    bands.update();

    render();

    pano.setup();

    txButton.setup();
    freqEncButton.setup();
    modeButton.setup();
    vfoButton.setup();
    stepButton.setup();
    memButton.setup();
    bandButton.setup();

    sMeter.setup();
    swrMeter.setup();

    freqEncButton.registerShortPressCallback(&switchRIT);
    freqEncButton.registerLongPressCallback(&switchBand);

    stepButton.registerShortPressCallback(&switchStep);
    bandButton.registerShortPressCallback(&switchBand);

    modeButton.registerShortPressCallback(&switchModulation);
    modeButton.registerLongPressCallback(&switchBacklight);

    vfoButton.registerShortPressCallback(&switchVFO);
    stepButton.registerShortPressCallback(&switchStep);

    buildMenu();
}
auto menuPreviousState = menu.isActive();


void renderTXUI() {
    tft.fillRect(0, FREQUENCY_Y, TFT_WIDTH, TFT_HEIGHT-FREQUENCY_Y, ST77XX_BLACK);
    displayMode();
    setFrequency();
    swrMeter.render();
}

void renderRXUI() {
    tft.fillRect(0, FREQUENCY_Y, TFT_WIDTH, TFT_HEIGHT-FREQUENCY_Y, ST77XX_BLACK);
    displayMode();
    displaySMeter();
    setFrequency();
    displayScale(true);
    sMeter.drawLevel(1);
    sMeter.drawLevel(12);
}


// MAIN LOOP ===================================================================
void loop() {

    analogWrite(BACKLIGHT_PIN, 180);

    txButton.loop();



    if (txButton.isPressed() && state.tx == 0) {
        state.tx = true;
        if (!menu.isActive()) {
            renderTXUI();
        }
    } else if (txButton.isReleased() && state.tx != 0) {
        state.tx = false;
        // getting back in rx
        if (!menu.isActive()) {
            renderRXUI();
        }
    }
    yield();

    if (menu.isActive() != menuPreviousState) {
        // we should render UI=
        menuPreviousState = menu.isActive();
    }

    if (state.tx) {
        swrMeter.loop();
    } else {
        auto currentMS = millis();


        if (loopMS != currentMS) {


            sMeter.loop();
            freqEncButton.loop();
            modeButton.loop();
            vfoButton.loop();
            stepButton.loop();
            memButton.loop();
            bandButton.loop();

            long int lastEncoderPosition = freqEncoder.read();
            if (lastEncoderPosition != encoderPosition && freqEncButton.isPressed()) {
                freqEncButton.disable();
            }
            if (lastEncoderPosition > encoderPosition + 2) {
                if (freqEncButton.isPressed()) {
                    changeFrequencyStep(-1);
                } else if (state.isRIT) {
                    if (state.RITFrequency > -9999) {
                        state.RITFrequency--;
                    }
                    if (!menu.isActive()) {
                        displayRIT();
                    }
                } else {
                    state.frequency -= state.step;

                }
                encoderPosition = lastEncoderPosition;
            } else if (lastEncoderPosition < encoderPosition - 2) {
                if (freqEncButton.isPressed()) {
                    changeFrequencyStep(1);
                } else if (state.isRIT) {
                    if (state.RITFrequency < 9999) {
                        state.RITFrequency++;
                    }
                    if (!menu.isActive()) {
                        displayRIT();
                    }
                } else {
                    state.frequency += state.step;

                }
                encoderPosition = lastEncoderPosition;
            }


            if (state.frequency > 30000000) {
                state.frequency = 100000;
            } else if (state.frequency < 100000) {
                state.frequency = 30000000;
            }

            if (oFrequency != state.frequency) {
                setFrequency();
            }

            loopMS = currentMS;
            // settle
            yield();
        }
        si5351.set_freq(
            static_cast<uint64_t>(pano.loop() + INTERMEDIATE_FREQUENCY)*100ULL,
            SI5351_CLK2
        );

    }

    yield();
}

