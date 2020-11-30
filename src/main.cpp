#include "config.h"

#include <Wire.h>

#include <Encoder.h>
#include <EEPROM.h>

#include <si5351.h>
#include <wiring_private.h>

#include "ui/SMeter.h"
#include "ui/Band.h"
#include "button.h"
#include "ui/Pano.h"
#include "swr.h"
#include "menu/Action.h"
#include "menu/Menu.h"
#include "Storage.h"
#include "ui/Spinner.h"
#include "Adafruit_MCP23017.h"

#include <CRC32.h>


volatile long setupCalls = 0;
volatile long encoderPosition = 0;
volatile unsigned long loopMS;
bool menuPreviousState = false;
//
Message subMenuScreen = MsgExit;
//
Encoder freqEncoder(ENCODER_LEFT_PIN, ENCODER_RIGHT_PIN); // pin (2 = D2, 3 = D3)
Button *freqEncButton = new Button(ENCODER_PUSH_PIN);
Si5351 *si5351 = new Si5351();


Button *txButton = new Button(TX_BTN_PIN);
Button *menuButton = new Button(MEM_BTN_PIN);   // PC5
Button *modeButton = new Button(MODE_BTN_PIN); // PC4
Button *vfoButton = new Button(VFO_BTN_PIN);   // PC3
Button *stepButton = new Button(STEP_BTN_PIN); // PA5
Button *bandButton = new Button(BAND_BTN_PIN); // PA4

SWRMeter swrMeter(SWR_REF_INPUT_PIN, SWR_FOR_INPUT_PIN);

Display *display = new Display(&tft);
SMeter sMeter(SMETER_INPUT_PIN, display);

Pano *pano = new Pano(PANO_INPUT_PIN, si5351, &state, display);
Adafruit_MCP23017 mcp;
Band *band = new Band(display, &state, &mcp);

Message mainMenuMessages[] = {
        MsgMemory,
        MsgSWR,
        MsgCW,
        MsgSettings,
        MsgAbout,
        MsgExit
};
Message settingsMenuMessages[] = {
        MsgIF,
        MsgSSBOffset,
        MsgDDSCalibration,
        MsgSWRCalibration,
        MsgTogglePano,
        MsgExit
};

Message memoryMenuMessages[] = {
        MsgLoadFromTheCell,
        MsgSaveToNewCell,
        MsgErase,
        MsgExit
};

Menu mainMenu(display, mainMenuMessages, 6);
Menu settingsMenu(display, settingsMenuMessages, 5);
Menu memoryMenu(display, memoryMenuMessages, 4);

Menu *currentMenu = &mainMenu;

Storage *storage = new Storage(0x50);


Spinner<uint32_t> *ifSpinner = new Spinner<uint32_t>(display);
Spinner<int32_t> *ssbSpinner = new Spinner<int32_t>(display);

size_t memoryCellIndex = 1;

int64_t getIntermediateFrequency() {
    int64_t fOffset;
    switch (state.mode) {
        case LSB:
            fOffset = settings.ssbOffset;
            break;
        case USB:
            fOffset = -settings.ssbOffset;
            break;
        case CW:
            fOffset = -settings.cwOffset;
            break;
        default:
            fOffset = 0;
    }
    return settings.iFrequency + fOffset;
}

void setFrequency() {
    oFrequency = state.frequency;
    uint64_t f = 0;

    // in a split mode we receive on one frequency and transmit on alternative frequency
    if (VFO_SPLIT == state.vfo) {
        if (state.tx) {
            f = state.altFrequency + getIntermediateFrequency();
        } else {
            f = state.frequency + getIntermediateFrequency() + (state.isRIT ? state.RITFrequency : 0);
        }
    } else {
        // normally we apply RIT offset in RX and transmit on a correct frequency
        // f = state.frequency + getIntermediateFrequency() + (state.isRIT && !state.tx ? state.RITFrequency : 0);
         //f = state.frequency + settings.iFrequency - settings.ssbOffset + (state.isRIT && !state.tx ? state.RITFrequency : 0);
         f = state.frequency + settings.iFrequency + (state.isRIT && !state.tx ? state.RITFrequency : 0);
        //f = state.frequency;
    }

    // @todo: add reverse in future
    si5351->set_freq(
            static_cast<uint64_t>(f) * 100ULL,
            SI5351_CLK0
    );
    yield();
    if (!currentMenu->isActive()) {
        displayFrequency();
        band->loop();
    }
}


void displayVFO() {
    Message m = MsgVFOA;
    switch (state.vfo) {
        case VFO_A:
            m = MsgVFOA;
            break;
        case VFO_B:
            m = MsgVFOB;
            break;
        case VFO_SPLIT:
            m = MsgVFOSPLIT;
            break;
    }
    display->drawRoundTextBox(
        TFT_QUOTER_WIDTH + 1u,
        0,
        TFT_QUOTER_WIDTH - 2u,
        15,
        m,
        COLOR_GRAY_MEDIUM,
        COLOR_DARK_GREEN

    );
}


void render() {
    tft.fillScreen(ST77XX_BLACK);
    displayModulation();
    displayMode();
    band->draw();
    changeFrequencyStep(0);
    displayFrequency();
    sMeter.setup();
    displayRIT();
    displayVFO();
}

/*
TX - must be separate, external 1k
BAND short, MENU long
MODE
RIT

*/
unsigned long bs;

void switchBand() {
    switch (subMenuScreen) {
        case MsgExit:
            band->next();
            break;
        default:;
            // nothing
    }
}


void setIntermediateFrequency() {
    si5351->set_freq(
            static_cast<uint64_t>(getIntermediateFrequency()) * 100ULL,
            SI5351_CLK1
    );
    yield();
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
    switch (subMenuScreen) {
        case MsgExit:
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
            setIntermediateFrequency();

            setFrequency();
            break;
        default:;
    }
}

void cycleAltFrequency() {
    uint32_t t = state.frequency;
    state.frequency = state.altFrequency;
    state.altFrequency = t;
}

void switchVFO() {
    if (subMenuScreen != MsgExit) {
        return;
    }
    switch (state.vfo) {
        case VFO_A:
            state.vfo = VFO_B;
            cycleAltFrequency();
        break;
        case VFO_B:
            state.vfo = VFO_SPLIT;
            cycleAltFrequency();
            break;
        case VFO_SPLIT:
            state.vfo = VFO_A;
            break;
    }

    displayVFO();
    setFrequency();
    setIntermediateFrequency();
}



void stepButtonShortClickCb() {
    switch (subMenuScreen) {
        case MsgIF:
            ifSpinner->changeStep();
            ifSpinner->loop();
            break;
        case MsgSSBOffset:
            ssbSpinner->changeStep();
            ssbSpinner->loop();
            break;
        default:
            changeFrequencyStep(1);
    }
}

void menuClick() {
    if (&(*currentMenu) != &mainMenu) {
        currentMenu = &mainMenu;
    }
    if (!currentMenu->isActive()) {
        currentMenu->setActive(true);
    }
    currentMenu->render();
}

void exitMenu() {
    currentMenu->exit();
}

void loadSettings() {
    MemorySettingsCell as;
    EEPROM.get(0, as);
    uint32_t asCrc = CRC32::calculate(&as.settings, 1);
    if (asCrc == as.crc) {
        settings.iFrequency = as.settings.iFrequency;
        settings.ssbOffset = as.settings.ssbOffset;
        settings.cwOffset = as.settings.cwOffset;
        settings.isPanoEnabled = as.settings.isPanoEnabled;
    }
    display->clear();
    render();
    setFrequency();
    setIntermediateFrequency();
}

void saveSettings() {
    MemorySettingsCell msc;
    msc.settings = settings;
    msc.crc = CRC32::calculate(
            &(msc.settings),
            1
    );
    EEPROM.put(0, msc);
}

void loadStateFromCell(size_t cellId) {
    display->clear();
    display->textxy(20, 30, MsgLoadFromTheCell, ST7735_WHITE, ST7735_BLACK);

    auto csz = (EEPROM.length() - sizeof(MemorySettingsCell)) / sizeof(MemoryStateCell);
    display->textxy(20, 40, String(csz).c_str(), ST7735_WHITE, ST7735_BLACK);

    MemoryStateCell mc;
    EEPROM.get(cellId * sizeof(mc) + sizeof(MemorySettingsCell), mc);

    uint32_t crc1 = CRC32::calculate(&mc.state, 1);

    if (crc1 == mc.crc) {
        // valid crc
        SetVolatileState(state, mc.state);
        display->textxy(20, 90, MsgOK, ST7735_GREEN, ST7735_BLACK);
    } else {
        display->textxy(20, 90, MsgInvalidCRC, ST7735_RED, ST7735_BLACK);
        delay(1000);
    }
    display->clear();
    render();
}

void saveStateToACell(size_t cellId) {
    display->clear();
    display->textxy(20, 70, MsgSaving, ST7735_WHITE, ST7735_BLACK);
    MemoryStateCell mc;
    mc.state = StateFromVolatile(state);
    mc.crc = CRC32::calculate(
            &(mc.state),
            1
    );
    EEPROM.put(cellId * sizeof(mc) + sizeof(MemorySettingsCell), mc);
    yield();
    delay(1000);
    display->clear();
    render();
}

void displayAbout() {
    display->clear();
    subMenuScreen = MsgAbout;
    display->textxy(20, 10, F("SSB85 Transceiver"), ST7735_WHITE, ST7735_BLACK);
    display->textxy(20, 20, F("Version: 1.0.0"), COLOR_MEDIUM_RED, ST7735_BLACK);
    display->textxy(74, 30, F("2019-05-07"), COLOR_MEDIUM_RED, ST7735_BLACK);
    display->textxy(1, 50, F("Details & updates online:"), COLOR_GRAY_MEDIUM, ST7735_BLACK);
    display->textxy(1, 65, F("zoonman.com/projects/ssb85"), COLOR_BRIGHT_BLUE, ST7735_BLACK);
    display->textxy(0, 110, F("Built by Philipp Tkachev"), COLOR_DARK_GREEN, ST7735_BLACK);
}


void displayAllEeprom() {

    display->drawRoundTextBox(
            0,
            0,
            display->tft->width(),
            MENU_ITEM_HEIGHT,
            subMenuScreen,
            ST77XX_WHITE,
            ST77XX_BLUE
    );

    auto memoryPage = (memoryCellIndex - 1) / MAX_MENU_ACTIONS_PER_SCREEN;

    auto lib = 1 + memoryPage * MAX_MENU_ACTIONS_PER_SCREEN;
    auto uib = 1 + (memoryPage + 1) * MAX_MENU_ACTIONS_PER_SCREEN;

    if (uib > 100) {
        uib = 100;
    }

    for (size_t menuIndex = lib; menuIndex < uib;) {
        MemoryStateCell mc;

        EEPROM.get(menuIndex * sizeof(mc), mc);
        uint32_t crc1 = CRC32::calculate(
                &mc.state,
                1
        );
        //String mt(menuIndex);

        uint16_t color = COLOR_GRAY_MEDIUM;
        char buffer[32];
        if (crc1 == mc.crc) {
            sprintf(buffer,
                    "%2d. %8lu %s %3s %+4d",
                    menuIndex,
                    (mc.state.isAltFrequency ? mc.state.altFrequency : mc.state.frequency),
                    (mc.state.isAltFrequency ? "B" : "A"),
                    ModeNames[mc.state.mode],
                    (mc.state.isRIT ? mc.state.RITFrequency : 0)
            );
        } else {
            sprintf(buffer,
                    "%2d. -- %10s",
                    menuIndex,
                    ""
            );
            color = COLOR_DARK_GRAY;
        }

        auto isActive = menuIndex == memoryCellIndex;
        String b1(buffer);
        display->drawRoundTextBox(
                MENU_ITEM_HEIGHT / 2,
                static_cast<uint8_t>((menuIndex - 1) % MAX_MENU_ACTIONS_PER_SCREEN * MENU_ITEM_HEIGHT)
                + MENU_ITEM_HEIGHT,
                static_cast<uint8_t>(display->tft->width() - MENU_ITEM_HEIGHT),
                MENU_ITEM_HEIGHT,
                &b1,
                isActive ? COLOR_BRIGHT_GREEN : color,
                isActive ? COLOR_DARK_GREEN : ST77XX_BLACK
        );
        menuIndex++;
    }
}

void eraseEeprom() {
    display->clear();
    display->drawRoundTextBox(0, 0, TFT_WIDTH, TFT_HEIGHT, MsgErasing, ST7735_WHITE, ST7735_BLACK);
    for (int i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 255);
    }
    display->clear();
    render();
}

void displayIntermediateFrequencySettings() {
    display->clear();
    subMenuScreen = MsgIF;
    display->drawRoundTextBox(0, 0, TFT_WIDTH, 22, MsgSSB85, ST7735_WHITE, ST7735_BLACK);
    ifSpinner->setLeft(5);
    ifSpinner->setLabel(MsgIF);
    ifSpinner->setTop(40);
    ifSpinner->setWidth(110);
    ifSpinner->setHeight(32);
    ifSpinner->setFocus(true);
    ifSpinner->setVisibility(true);
    ifSpinner->setValue(settings.iFrequency);
    ifSpinner->draw();
}

void displaySSBOffsetSettings() {
    display->clear();
    subMenuScreen = MsgSSBOffset;
    display->drawRoundTextBox(0, 0, TFT_WIDTH, 22, MsgSSB85, ST7735_WHITE, ST7735_BLACK);
    ssbSpinner->setLeft(5);
    ssbSpinner->setLabel(MsgSSBOffset);
    ssbSpinner->setTop(40);
    ssbSpinner->setWidth(110);
    ssbSpinner->setHeight(32);
    ssbSpinner->setFocus(true);
    ssbSpinner->setVisibility(true);
    ssbSpinner->setValue(settings.ssbOffset);
    ssbSpinner->draw();
}


class App {
private:
    Task **tasks;
    size_t length = 0;
    size_t nextTask = 0;

    void AddWidget(Task *task) {
        size_t nl = ++this->length;
        this->tasks = (Task **) realloc(this->tasks, nl * sizeof(*task));
        this->tasks[this->length - 1] = task;
    }

    void loop() {
        if (--this->nextTask > -1) {
            this->tasks[this->nextTask]->loop();
        } else {
            this->nextTask = this->length;
        }
    }
};


void callMenuFunc(Message m) {
    switch (m) {
        case MsgAbout:
            currentMenu->exit();
            displayAbout();
            break;
        case MsgMemory:
            currentMenu = &memoryMenu;
            currentMenu->setActive(true);
            currentMenu->render();
            break;
        case MsgSettings:
            currentMenu = &settingsMenu;
            currentMenu->setActive(true);
            currentMenu->render();
            break;
        case MsgIF:
            currentMenu->exit();
            displayIntermediateFrequencySettings();
            break;
        case MsgSaveToNewCell:
            currentMenu->exit();
            display->clear();
            if (subMenuScreen == MsgSaveToNewCell) {
            } else {
                subMenuScreen = MsgSaveToNewCell;
                displayAllEeprom();
            }
            break;
        case MsgLoadFromTheCell:
            currentMenu->exit();
            display->clear();
            if (subMenuScreen == MsgLoadFromTheCell) {
            } else {
                subMenuScreen = MsgLoadFromTheCell;
                displayAllEeprom();
            }
            break;
        case MsgErase:
            currentMenu->exit();
            eraseEeprom();
            break;
        case MsgTogglePano:
            state.isPanoEnabled = !state.isPanoEnabled;
            currentMenu->exit();
            break;
        case MsgSSBOffset:
            currentMenu->exit();
            displaySSBOffsetSettings();
            break;
        case MsgExit:
        case MsgCW:
        case MsgDDSCalibration:
        case MsgSWRCalibration:
            currentMenu->exit();
            if (currentMenu == &settingsMenu) {
                currentMenu = &mainMenu;
            }
            display->clear();
            render();
            break;
        default:;
    }
}

void encoderClickHandler() {
    if (currentMenu->isActive()) {
        callMenuFunc(currentMenu->getActiveMessage());
    } else if (subMenuScreen != MsgExit) {
        //
        switch (subMenuScreen) {
            case MsgLoadFromTheCell:
                loadStateFromCell(memoryCellIndex);
                subMenuScreen = MsgExit;
                break;
            case MsgSaveToNewCell:
                saveStateToACell(memoryCellIndex);
                subMenuScreen = MsgExit;
                break;
            case MsgIF:
                settings.iFrequency = ifSpinner->getValue();
                display->clear();
                saveSettings();
                saveStateToACell(0);
                subMenuScreen = MsgExit;
                break;
            case MsgSSBOffset:
                settings.ssbOffset = ssbSpinner->getValue();
                display->clear();
                saveSettings();
                saveStateToACell(0);
                subMenuScreen = MsgExit;
                break;
            case MsgAbout:
                display->clear();
                render();
                subMenuScreen = MsgExit;
                break;
            default:;
        }
    } else {
        state.isRIT = !state.isRIT;
        displayRIT();
        setFrequency();
    }
}

void encoderCW() {
    switch (subMenuScreen) {
        case MsgLoadFromTheCell:
        case MsgSaveToNewCell:
            if (memoryCellIndex < 97) {
                memoryCellIndex++;
                displayAllEeprom();
            }
            break;
        case MsgIF:
            ifSpinner->inc();
            ifSpinner->loop();
            settings.iFrequency = ifSpinner->getValue();
            setIntermediateFrequency();
            setFrequency();
            break;
        case MsgSSBOffset:
            ssbSpinner->inc();
            ssbSpinner->loop();
            settings.ssbOffset = ssbSpinner->getValue();
            setIntermediateFrequency();
            setFrequency();
            break;
        default:;
    }
}

void encoderCCW() {
    switch (subMenuScreen) {
        case MsgLoadFromTheCell:
        case MsgSaveToNewCell:

            if (memoryCellIndex > 1) {
                memoryCellIndex--;
                displayAllEeprom();
            }
            break;
        case MsgIF:
            ifSpinner->dec();
            ifSpinner->loop();
            settings.iFrequency = ifSpinner->getValue();
            setIntermediateFrequency();
            setFrequency();
            break;
        case MsgSSBOffset:
            ssbSpinner->dec();
            ssbSpinner->loop();
            settings.ssbOffset = ssbSpinner->getValue();
            setIntermediateFrequency();
            setFrequency();

            break;
        default:;
    }
}

/****************************************************************
 *
 *   SETUP
    // put your setup code here, to run once:
 *
 ****************************************************************/

void setup() {
    setupCalls++;
    // delay(1000);

    pinMode(28, INPUT);
    analogReference(INTERNAL2V56);

    pinMode(BACKLIGHT_PIN, OUTPUT);
    digitalWrite(BACKLIGHT_PIN, HIGH);

    pinMode(TFT_BACKLIGHT_PIN, OUTPUT);

    si5351->init(SI5351_CRYSTAL_LOAD_0PF, 0, 0);
    yield();
    tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
    tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(1);

    loopMS = millis();

    band->setVisibility(true);

    //
    //
    //Serial.begin(9600);
    //Serial.println("TWBR");
    //Serial.println(TWBR);
    //Serial.println(TWSR);
    // TWBR
    // Wire.

//    Wire.setClock(400000);
    //Serial.println("--");
    //Serial.println(TWBR);
    //Serial.println(TWSR);
    si5351->output_enable(SI5351_CLK0, 1);
    si5351->drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
    yield();
    si5351->output_enable(SI5351_CLK1, 1);
    yield();
    si5351->output_enable(SI5351_CLK2, 1);

    si5351->drive_strength(SI5351_CLK1, SI5351_DRIVE_2MA);
    si5351->drive_strength(SI5351_CLK2, SI5351_DRIVE_2MA);
    yield();

/**/
    // state.frequency = START_FREQUENCY;
    // state.altFrequency = START_FREQUENCY + 1000;
    loadSettings();
    loadStateFromCell(0);

    setIntermediateFrequency();
    setFrequency();

    freqEncoder.write(encoderPosition);

    setFrequency();

    render();
    band->loop();

    txButton->setup();
    freqEncButton->setup();
    modeButton->setup();
    vfoButton->setup();
    stepButton->setup();
    menuButton->setup();
    bandButton->setup();

    sMeter.setup();
    swrMeter.setup();

    freqEncButton->registerShortPressCallback(&encoderClickHandler);
    freqEncButton->registerLongPressCallback(&switchBand);

    stepButton->registerShortPressCallback(&stepButtonShortClickCb);
    bandButton->registerShortPressCallback(&switchBand);

    modeButton->registerShortPressCallback(&switchModulation);
    modeButton->registerLongPressCallback(&switchBacklight);

    vfoButton->registerShortPressCallback(&switchVFO);
    // stepButton->registerShortPressCallback(&stepButtonShortClickCb);


    menuButton->registerShortPressCallback(&menuClick);
    delay(10);

    mcp.begin(0);

    mcp.pinMode(0, OUTPUT);

    mcp.pinMode(1, OUTPUT);
    mcp.digitalWrite(0, HIGH);
    mcp.digitalWrite(1, HIGH);

    mcp.pinMode(7, OUTPUT);
    mcp.pinMode(9, OUTPUT);

    mcp.digitalWrite(1, LOW);
    mcp.digitalWrite(7, LOW);
    mcp.digitalWrite(9, HIGH);

    delay(10);
    //storage->loadState(&state, 0);
    // delay(10);

    menuPreviousState = currentMenu->isActive();
    // turn on led
    digitalWrite(TFT_BACKLIGHT_PIN, HIGH);

    band->draw();
}


void renderTXUI() {
    mcp.digitalWrite(0, HIGH);
    mcp.digitalWrite(1, LOW);

    tft.fillRect(0, FREQUENCY_Y, TFT_WIDTH, TFT_HEIGHT - FREQUENCY_Y, ST77XX_BLACK);
    displayMode();
    setFrequency();
    swrMeter.render();
}

void renderRXUI() {
    mcp.digitalWrite(0, LOW);
    mcp.digitalWrite(1, HIGH);

    tft.fillRect(0, FREQUENCY_Y, TFT_WIDTH, TFT_HEIGHT - FREQUENCY_Y, ST77XX_BLACK);
    displayMode();
    displaySMeter();
    setFrequency();
    // displayScale(true);
    sMeter.drawLevel(1);
    sMeter.drawLevel(12);
    band->loop();
}


// MAIN LOOP ===================================================================
void loop() {
    // return;
    txButton->loop();

    if (txButton->isPressed() && state.tx == 0) {
        state.tx = true;
        if (!currentMenu->isActive() && subMenuScreen == MsgExit) {
            renderTXUI();
        }
    } else if (txButton->isReleased() && state.tx != 0 && subMenuScreen == MsgExit) {
        state.tx = false;
        // getting back in rx
        if (!currentMenu->isActive()) {
            renderRXUI();
        }
    }
    yield();

    if (currentMenu->isActive() != menuPreviousState) {
        // we should render UI=
        menuPreviousState = currentMenu->isActive();
    }

    if (state.tx) {
        swrMeter.loop();
    } else {
        auto currentMS = millis();

        // save cycles, we don't have to evaluate all that stuff with 16MHz frequency
        if (currentMS - loopMS > 10) {

            freqEncButton->loop();
            modeButton->loop();
            vfoButton->loop();
            stepButton->loop();
            menuButton->loop();
            bandButton->loop();

            long int lastEncoderPosition = freqEncoder.read();
            if (lastEncoderPosition != encoderPosition && freqEncButton->isPressed()) {
                freqEncButton->disable();
            }
            // turning the knob counter-clockwise
            if (lastEncoderPosition > encoderPosition + 2) {
                if (currentMenu->isActive()) {
                    currentMenu->down();
                } else if (subMenuScreen != MsgExit) {
                    encoderCCW();
                } else if (freqEncButton->isPressed()) {
                    changeFrequencyStep(-1);
                } else if (state.isRIT) {
                    if (state.RITFrequency > -9999) {
                        state.RITFrequency--;
                    }
                    if (!currentMenu->isActive()) {
                        displayRIT();
                    }
                } else {
                    state.frequency -= state.step;
                    band->loop();
                }
                encoderPosition = lastEncoderPosition;
            } else if (lastEncoderPosition < encoderPosition - 2) {
                // turning the knob clockwise
                if (currentMenu->isActive()) {
                    currentMenu->up();
                } else if (subMenuScreen != MsgExit) {
                    encoderCW();
                } else if (freqEncButton->isPressed()) {
                    changeFrequencyStep(1);
                } else if (state.isRIT) {
                    if (state.RITFrequency < 9999) {
                        state.RITFrequency++;
                    }
                    if (!currentMenu->isActive()) {
                        displayRIT();
                    }
                } else {
                    state.frequency += state.step;
                    band->loop();
                }
                encoderPosition = lastEncoderPosition;
            }

            if (state.frequency > UPPER_RX_BOUND) {
                state.frequency = LOWER_RX_BOUND;
            } else if (state.frequency < LOWER_RX_BOUND) {
                state.frequency = UPPER_RX_BOUND;
            }

            if (oFrequency != state.frequency) {
                setFrequency();
            }

            if (!currentMenu->isActive()) {
                yield();/*
                // A4 = PA3 = ADC3
                // set  ADC Multiplexer Selection Register
                ADMUX = (_BV(MUX0) | _BV(MUX1) // select ADC3 input
                         | _BV(REFS0) | _BV(REFS1)); // pull 2.65 V REF
                cbi(ADMUX, ADLAR); // turn of left adjust
                cbi(ADMUX, MUX3); // turn off gain
                cbi(ADMUX, MUX4); // turn off differential input

                cbi(ADCSRA, ADATE);
                cbi(ADCSRA, ADIE);

                sbi(ADCSRA, ADEN); // enable ADC
                cbi(PRR0, PRADC); // enable power reduction ADC
                sbi(ADCSRA, ADSC); // turn on ADC
                while (bit_is_set(ADCSRA, ADSC));
                uint8_t low = ADCL;
                uint8_t high = ADCH;
                cbi(ADCSRA, ADEN);
                uint16_t voltage = (((high << 8) | low) & 0x03FF) + 1;
                auto av = (float) voltage / 37.5;
                auto str1 = String(av, 2);
                str1.concat("V");
                if (subMenuScreen == MsgExit) {
                    display->textxy(50, RIT_Y, &str1, COLOR_DARK_GREEN, ST7735_BLACK);
                }*/
            }

            loopMS = currentMS;
            // settle
            yield();
        }
        if (!currentMenu->isActive() && subMenuScreen == MsgExit) {
            sMeter.loop();
            pano->loop();
            band->loop();
        }
        yield();
    }
/*
 * Render pool
 *
 * registers rendering elements
 * has flag to re-render
 * cycles through loop
 * renders one component per cycle
 *
 * **/
    yield();
}

