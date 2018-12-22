// #define ENCODER_OPTIMIZE_INTERRUPTS
#define TWI_FREQ 400000L

#define WIRING 1
#define CORE_NUM_INTERRUPT 6
#define CORE_NUM_INTERRUPT 6

// #include <Encoder.h>

#include <si5351.h>

#include "smeter.h"
#include "bands.h"
#include "button.h"

long encoderPosition  = 0;

//
// Encoder freqEncoder(2, 3); // pin (2 = D2, 3 = D3)
// Si5351 si5351;

Bands bands;

SMeter sMeter(A0);
Button freqEncButton(PD4);
Button txButton(PD5);

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
    state.isRIT = !state.isRIT;
    displayRIT();
    setFrequency();
}

#define WATERFALL_ROWS 6
#define WATERFALL_COLS 80
uint8_t PXLT[WATERFALL_COLS * WATERFALL_ROWS];

// SETUP -----------------------------------------------------------------------
void setup() {

    //PORTD
    pinMode(13, OUTPUT);
    digitalWrite(13, 0);


    /*

    tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
    // put your setup code here, to run once:
    pinMode(LED_BUILTIN, OUTPUT);
    tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(1);
    //
    // Serial.begin(9600);

    /*
    si5351.init(SI5351_CRYSTAL_LOAD_0PF, 0, 0);
    si5351.output_enable(SI5351_CLK0, 1);
    si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_4MA);
    si5351.output_enable(SI5351_CLK2, 1);
    si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_4MA);
* /

    state.frequency = START_F;

    setFrequency();
    // freqEncoder.write(encoderPosition);

    setFrequency();

    bands.update();

    render();

    txButton.setup();
    freqEncButton.setup();

    render();
    sMeter.setup();

    freqEncButton.registerShortPressCallback(&switchStep);
    freqEncButton.registerLongPressCallback(&switchBand);*/
}

uint8_t row = 0, col = 0;

// MAIN LOOP ===================================================================
void loop() {

    pinMode(13, OUTPUT);
    digitalWrite(13, 0);

    /*
    sMeter.loop();

    txButton.loop();
    freqEncButton.loop();

    long int lastEncoderPosition = 1; //freqEncoder.read();
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
    } else if (txButton.isReleased() && state.tx != 0) {
        state.tx = false;
        displayMode();
        setFrequency();
    }

    //displaySWR();

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
        );* /
        PXLT[col + WATERFALL_COLS * row] = static_cast<uint8_t>(analogRead(A0) / 4);
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
                //tft.drawPixel(x, 68 - y*2, rgb565);
                tft.fillRect(x*2, 68 - y*2, 2, 2, rgb565);

            }
            if (--rowIndex == -1) {
                rowIndex = WATERFALL_ROWS - 1;
            };
        }
        if (++row >= WATERFALL_ROWS) {
            row = 0;
        }
    }
*/
}

