#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#include <si5351.h>

#include "smeter.h"
#include "bands.h"
#include "button.h"

long encoderPosition  = 0;

//
Encoder freqEncoder(2, 3); // pin (2 = D2, 3 = D3)
Si5351 si5351;

Bands bands;

SMeter sMeter(A0);
Button freqEncButton(PD4);
Button txButton(PD5);

void setFrequency() {
    oFrequency = state.frequency;
    si5351.set_freq(
            (state.frequency + state.isRIT && !state.tx ? state.RITFrequency : 0) * 100ULL,
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
    sMeter.drawLevel(1);
    sMeter.drawLevel(12);
    displayRIT();
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
}

void switchStep() {
    state.isRIT = !state.isRIT;
    displayRIT();
    setFrequency();
}

// SETUP -----------------------------------------------------------------------
void setup() {
    tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
    // put your setup code here, to run once:
    pinMode(LED_BUILTIN, OUTPUT);
    tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(1);
    //
    // Serial.begin(9600);
    si5351.init(SI5351_CRYSTAL_LOAD_0PF, 0, 0);

    state.frequency = START_F;

    setFrequency();
    freqEncoder.write(encoderPosition);

    setFrequency();

    bands.update();

    render();

    txButton.setup();
    freqEncButton.setup();

    render();
    sMeter.setup();

    freqEncButton.registerShortPressCallback(&switchStep);
    freqEncButton.registerLongPressCallback(&switchBand);
}

// MAIN LOOP ===================================================================
void loop() {

    sMeter.loop();

    txButton.loop();
    freqEncButton.loop();

    long int lastEncoderPosition = freqEncoder.read();
    if (lastEncoderPosition != encoderPosition && freqEncButton.isPressed()) {
        freqEncButton.cancelHandlers();
    }
    if (lastEncoderPosition > encoderPosition + 2) {
        if (freqEncButton.isPressed()) {
            changeFrequencyStep(-1);
        } else if (state.isRIT) {
            if (state.RITFrequency > -9999) {
                state.RITFrequency--;
            }
            displayRIT();
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
            displayRIT();
        } else {
            state.frequency += state.step;
        }
        encoderPosition = lastEncoderPosition;
    }


    if (oFrequency != state.frequency) {
        setFrequency();
    }

    if (txButton.isPressed() && state.tx == 0) {
        state.tx = true;
        displayMode();
        setFrequency();
    } else if (txButton.isReleased() && state.tx != 0) {
        state.tx = false;
        displayMode();
        setFrequency();
    }
}
