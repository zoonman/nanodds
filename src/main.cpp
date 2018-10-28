#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#include <si5351.h>

#include "smeter.h"
#include "bands.h"
#include "button.h"


uint8_t freqMultiplier = 5;
long encoderPosition  = 0;

//
Encoder myEnc(2, 3); // pin (2 = D2, 3 = D3)
Si5351 si5351;
Bands bands;
SMeter sMeter(A0);
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
    bands.update();
    displayStep(0);
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

    sMeter.setup();
    //
    Serial.begin(9600);
    si5351.init(SI5351_CRYSTAL_LOAD_0PF, 0, 0);

    state.frequency = START_F;

    setFrequency();
    myEnc.write(encoderPosition);

    setFrequency();

    bands.update();

    render();

    pinMode(PD4, INPUT_PULLUP);
    txButton.setup();
}

// MAIN LOOP ===================================================================
void loop() {

    sMeter.loop();

    txButton.loop();

    int pinD4 = digitalRead(PD4);

    long int lastEncoderPosition = myEnc.read();
    if (lastEncoderPosition > encoderPosition + 2) {
        if (pinD4 == LOW) {
            displayStep(-1);
            delay(500);
            millis();

        } else {
            state.frequency -= state.step;
            encoderPosition = lastEncoderPosition;
        }
    } else if (lastEncoderPosition < encoderPosition - 2) {
        if (pinD4 == LOW) {
            displayStep(1);
            delay(500);
        } else {
            state.frequency += state.step;
            encoderPosition = lastEncoderPosition;
        }
    } else if (pinD4 == LOW) {
        bands.next();
        delay(100);
    }
    if (oFrequency != state.frequency) {
        setFrequency();
    }
/*
    int pinD5 = digitalRead(PD5);
    if (pinD5 == LOW && state.tx == 0) {
        state.tx = true;
        displayMode();
    } else if (pinD5 == HIGH && state.tx != 0) {
        state.tx = false;
        displayMode();
    }*/
}
