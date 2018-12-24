#define ENCODER_OPTIMIZE_INTERRUPTS
#define TWI_FREQ 400000L

#define TX_BTN_PIN     6
#define MEM_BTN_PIN   16
#define MODE_BTN_PIN  17
#define VFO_BTN_PIN   18
#define STEP_BTN_PIN  26
#define BAND_BTN_PIN  27
#define BACKLIGHT_PIN  9

#define SMETER_INPUT_PIN 31

#define SWR_REF_INPUT_PIN 31
#define SWR_FOR_INPUT_PIN 31

#define WATERFALL_ROWS  16
#define WATERFALL_COLS 158

#define ENCODER_LEFT_PIN   2
#define ENCODER_RIGHT_PIN  3
#define ENCODER_PUSH_PIN  22

#include <Encoder.h>

#include <si5351.h>

#include "smeter.h"
#include "bands.h"
#include "button.h"

volatile long encoderPosition  = 0;

//
Encoder freqEncoder(ENCODER_LEFT_PIN, ENCODER_RIGHT_PIN); // pin (2 = D2, 3 = D3)
Button freqEncButton(ENCODER_PUSH_PIN);
// Si5351 si5351;

Bands bands;

SMeter sMeter(SMETER_INPUT_PIN);
Button txButton(TX_BTN_PIN);
Button memButton(MEM_BTN_PIN);   // PC3
Button modeButton(MODE_BTN_PIN); // PC4
Button vfoButton(VFO_BTN_PIN);   // PC5
Button stepButton(STEP_BTN_PIN); // PA5
Button bandButton(BAND_BTN_PIN); // PA4

void setFrequency() {
    oFrequency = state.frequency;
    /*
    si5351.set_freq(
        static_cast<uint64_t>(state.frequency + INTERMEDIATE_FREQUENCY + (state.isRIT && !state.tx ? state.RITFrequency : 0))*100ULL,
        SI5351_CLK0
    );
    */
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
    sMeter.drawLevel(1);
    sMeter.drawLevel(12);
    displayRIT();
    displayVFO();
    //displaySWR();
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

void switchStep() {
    //
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
    digitalWrite(BACKLIGHT_PIN, led);
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

volatile uint8_t PXLT[WATERFALL_COLS * WATERFALL_ROWS];

// SETUP -----------------------------------------------------------------------
void setup() {

    pinMode(9, OUTPUT);

    //PORTD
    pinMode(7, OUTPUT);
    digitalWrite(7, 1);

    tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

    // put your setup code here, to run once:
    tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(1);
    //
    // Serial.begin(9600);
/*
    /*
    si5351.init(SI5351_CRYSTAL_LOAD_0PF, 0, 0);
    si5351.output_enable(SI5351_CLK0, 1);
    si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_4MA);
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

    txButton.setup();
    freqEncButton.setup();
    modeButton.setup();
    vfoButton.setup();
    stepButton.setup();
    memButton.setup();
    bandButton.setup();

    render();
    sMeter.setup();

    freqEncButton.registerShortPressCallback(&switchRIT);
    freqEncButton.registerLongPressCallback(&switchBand);

    stepButton.registerShortPressCallback(&switchStep);
    bandButton.registerShortPressCallback(&switchBand);

    modeButton.registerShortPressCallback(&switchModulation);
    modeButton.registerLongPressCallback(&switchBacklight);

    vfoButton.registerShortPressCallback(&switchVFO);
}

uint8_t row = 0, col = 0;

// MAIN LOOP ===================================================================
void loop() {

    sMeter.loop();
    txButton.loop();
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
        displaySWR();
    } else if (txButton.isReleased() && state.tx != 0) {
        state.tx = false;
        displayMode();
        displaySMeter();
        setFrequency();
    }

    uint16_t fStep; // kHz
    uint32_t panoFreq;

    if (state.band != BANDS) {
        fStep = BandsBounds[state.band].width * 1000 / WATERFALL_COLS;
        panoFreq = BandsBounds[state.band].start * 1000;
    } else {
        panoFreq = state.frequency - 100000;
        fStep = 625;
    }

    //for (uint8_t col = 0; col < TFT_WIDTH; col++) {
        panoFreq = panoFreq + fStep * col;
    /*
        si5351.set_freq(
                static_cast<uint64_t>(panoFreq + INTERMEDIATE_FREQUENCY)*100ULL,
                SI5351_CLK2
        );*/
        PXLT[col + WATERFALL_COLS * row] = static_cast<uint8_t>(analogRead(5) / 4);
    //}
    if (++col >= WATERFALL_COLS) {
        col = 0;

        int8_t rowIndex = row;
        for (uint8_t y = 0; y < WATERFALL_ROWS; y++) {
            // render it
            for (uint8_t x = 0;x < WATERFALL_COLS; x++) {
                uint8_t rgb8 = PXLT[x + WATERFALL_COLS * rowIndex];
                uint16_t rgb565;
                uint8_t red = (rgb8 & 0xC0) >> 6;
                uint8_t green = (rgb8 & 0x38) >> 3;
                uint8_t blue = (rgb8 & 0x07);
                rgb565 = ((red) << 15) | ((green) << 5) | (blue);
                //tft.drawPixel(x, 68 - y*2 + 1, rgb565);
                tft.drawPixel(x+1, SCALE_Y - y - GRID, rgb565);
                //tft.fillRect(x*2, 68 - y*2, 2, 2, rgb565);

            }
            if (--rowIndex == -1) {
                rowIndex = WATERFALL_ROWS - 1;
            };
        }
        if (++row >= WATERFALL_ROWS) {
            row = 0;
        }
    }
    tft.drawRect(0, SCALE_Y - WATERFALL_ROWS - GRID, TFT_WIDTH, WATERFALL_ROWS+2, COLOR_DARK_GRAY);

}

