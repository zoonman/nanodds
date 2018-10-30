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
    si5351.set_freq(state.frequency * 100ULL, SI5351_CLK0);
    displayFrequency();
    bands.update();
    displayScale();
}

void render() {
    tft.fillScreen(ST77XX_BLACK);
    displayModulation();
    displayMode();
    bands.render();
    displayStep(-1);
    displayScale();
    displayFrequency();
    sMeter.drawLevel(1);
    sMeter.drawLevel(12);
}
/*
TX - must be separate, external 1k
BAND short, MENU long
MODE
RIT

*/
unsigned long bs;
// SETUP -----------------------------------------------------------------------
void setup() {
    tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
    // put your setup code here, to run once:
    pinMode(LED_BUILTIN, OUTPUT);

    tft.fillScreen(ST77XX_BLACK);

    tft.setRotation(1);

    //
    Serial.begin(9600);
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
}

// MAIN LOOP ===================================================================
void loop() {

    sMeter.loop();

    txButton.loop();
    freqEncButton.loop();


    long int lastEncoderPosition = freqEncoder.read();
    if (lastEncoderPosition > encoderPosition + 2) {
        if (freqEncButton.isPressed()) {
            // displayStep(-1);
        } else {
            state.frequency -= state.step;
            encoderPosition = lastEncoderPosition;
        }
    } else if (lastEncoderPosition < encoderPosition - 2) {
        if (freqEncButton.isPressed()) {
            // displayStep(1);
        } else {
            state.frequency += state.step;
            encoderPosition = lastEncoderPosition;
        }
    }

    if (freqEncButton.isLongPress()) {
        displayStep(1);
    } else if (freqEncButton.isShortPress()) {
        bands.next();
    }

    if (oFrequency != state.frequency) {
        setFrequency();
    }

    if (txButton.isPressed() && state.tx == 0) {
        state.tx = true;
        displayMode();
    } else if (txButton.isReleased() && state.tx != 0) {
        state.tx = false;
        displayMode();
    }
}
