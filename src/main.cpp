
#include "config.h"
#include <Wire.h>

#include <EEPROM.h>
#include <si5351.h>
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
#include <avr/wdt.h>
#include <RotaryEncoder.h>
#include <ui/Frequency.h>

volatile long setupCalls = 0;
volatile long encoderPosition = 0;
volatile unsigned long loopMS;
bool menuPreviousState = false;
//
Message subMenuScreen = MsgExit;

#define ENCODER_DO_NOT_USE_INTERRUPTS
/// #define INPUT_PULLUP
// #define USE_FAST_PINIO

// Encoder freqEncoder(ENCODER_LEFT_PIN, ENCODER_RIGHT_PIN); // pin (2 = D2, 3 = D3)
RotaryEncoder freqEncoder(ENCODER_RIGHT_PIN, ENCODER_LEFT_PIN); // pin (2 = D2, 3 = D3)
Button *freqEncButton = new Button(ENCODER_PUSH_PIN);
Si5351 *si5351 = new Si5351();


Button *txButton = new Button(TX_BTN_PIN);
Button *menuButton = new Button(MEM_BTN_PIN);   // PC5
Button *modeButton = new Button(MODE_BTN_PIN); // PC4
Button *vfoButton = new Button(VFO_BTN_PIN);   // PC3
Button *stepButton = new Button(STEP_BTN_PIN); // PA5
Button *bandButton = new Button(BAND_BTN_PIN); // PA4

SWRMeter swrMeter(SWR_REF_INPUT_PIN, SWR_FOR_INPUT_PIN);

Display *display = new Display(tft);
SMeter *sMeter = new SMeter(SMETER_INPUT_PIN, display);

Frequency *frequency = new Frequency(&settings, si5351, &state, display);

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

/*
TX - must be separate, external 1k
BAND short, MENU long
MODE
RIT

*/

void switchBand() {
    switch (subMenuScreen) {
        case MsgExit:
            band->next();
            break;
        default:;
            // nothing
    }
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
            frequency->updatePllFrequency();
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
    frequency->updatePllFrequency();
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
            frequency->changeFrequencyStep(1);
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
    frequency->updatePllFrequency();
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
        delay(100);
    }
    display->clear();
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
    delay(100);
    display->clear();
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


void renderTXUI() {
    mcp.digitalWrite(0, LOW);
    mcp.digitalWrite(1, HIGH);

    display->clear();

    displayMode();
    displayModulation();
    displayVFO();

    frequency->updatePllFrequency();
    frequency->setVisibility(true);
    frequency->scheduleRedraw(Full);

    displayModulation();
    swrMeter.render();
    sMeter->setVisibility(false);
    pano->setVisibility(false);
}

void renderRXUI() {
    mcp.digitalWrite(0, HIGH);
    mcp.digitalWrite(1, LOW);

    display->clear();

    displayMode();
    displayModulation();
    displayVFO();

    pano->setVisibility(state.isPanoEnabled);

    sMeter->setVisibility(true);
    sMeter->scheduleRedraw(Full);

    band->setVisibility(true);
    band->scheduleRedraw(Full);

    frequency->updatePllFrequency();
    frequency->setVisibility(true);
    frequency->scheduleRedraw(Full);
}

void eraseEeprom() {
    display->clear();
    display->drawRoundTextBox(0, 0, TFT_WIDTH, TFT_HEIGHT, MsgErasing, ST7735_WHITE, ST7735_RED);
    for (int i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 255);
    }
    display->clear();
    renderRXUI();
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
            renderRXUI();
            break;
        default:;
    }
}

void encoderTick() {
    freqEncoder.tick();
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
                renderRXUI();
                break;
            case MsgSaveToNewCell:
                saveStateToACell(memoryCellIndex);
                subMenuScreen = MsgExit;
                renderRXUI();
                break;
            case MsgIF:
                settings.iFrequency = ifSpinner->getValue();
                display->clear();
                saveSettings();
                saveStateToACell(0);
                subMenuScreen = MsgExit;
                renderRXUI();
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
                subMenuScreen = MsgExit;
                renderRXUI();
                break;
            default:;
        }
    } else {
        state.isRIT = !state.isRIT;
        frequency->updatePllFrequency();
        frequency->scheduleRedraw(Full);
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
            frequency->updatePllFrequency();
            break;
        case MsgSSBOffset:
            ssbSpinner->inc();
            ssbSpinner->loop();
            settings.ssbOffset = ssbSpinner->getValue();
            frequency->updatePllFrequency();
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
            frequency->updatePllFrequency();
            break;
        case MsgSSBOffset:
            ssbSpinner->dec();
            ssbSpinner->loop();
            settings.ssbOffset = ssbSpinner->getValue();
            frequency->updatePllFrequency();
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
    tft->initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
    tft->fillScreen(ST77XX_BLACK);
    tft->setRotation(1);
    ///
    setupCalls++;
    // turn on buttons backlight
    pinMode(BACKLIGHT_PIN, OUTPUT);
    digitalWrite(BACKLIGHT_PIN, LOW);
    // and turn off led backlight
    // this will indicate that we booting
    pinMode(TFT_BACKLIGHT_PIN, OUTPUT);
    digitalWrite(TFT_BACKLIGHT_PIN, LOW);

    // turn on generator
    // will need some time for init procedure
    si5351->init(SI5351_CRYSTAL_LOAD_0PF, 0, 0);
    yield();
    wdt_reset();
    yield();
    delay(10);
    wdt_reset();



    // this takes some time so we reset watchdog once in a while

    wdt_reset();
    yield();
    si5351->output_enable(SI5351_CLK0, 1);
    si5351->drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
    si5351->output_enable(SI5351_CLK1, 1);
    si5351->output_enable(SI5351_CLK2, 1);
    si5351->drive_strength(SI5351_CLK1, SI5351_DRIVE_2MA);
    si5351->drive_strength(SI5351_CLK2, SI5351_DRIVE_2MA);
    yield();
    wdt_reset();
    // tft->fillScreen(ST77XX_BLUE);
    mcp.begin();
    yield();
    for (int p = 0; p < 16;p++) {
        wdt_reset();
        mcp.pinMode(p, OUTPUT);
    }
    yield();
    mcp.writeGPIOAB(0x301); // GPA0=1, GPB1=1
//    mcp.digitalWrite(0, HIGH);
//    mcp.digitalWrite(1, HIGH);
//
//    mcp.pinMode(7, OUTPUT);
//    mcp.pinMode(9, OUTPUT);
//
//    mcp.digitalWrite(1, LOW);
//    mcp.digitalWrite(7, LOW);
//    mcp.digitalWrite(9, HIGH);
    wdt_reset();
    yield();

    loopMS = millis();
    band->setVisibility(true);
    loadSettings();
    loadStateFromCell(0);

    frequency->updatePllFrequency();

    wdt_reset();

    analogReference(INTERNAL1V1);

    // render();

    wdt_reset();

    band->loop();
    txButton->setup();
    freqEncButton->setup();
    modeButton->setup();
    vfoButton->setup();
    stepButton->setup();
    menuButton->setup();
    bandButton->setup();

    wdt_reset();

    sMeter->setup();
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

    menuPreviousState = currentMenu->isActive();

    // we done with initialization
    // turn on TFT backlight
    digitalWrite(TFT_BACKLIGHT_PIN, HIGH);
    digitalWrite(BACKLIGHT_PIN, HIGH);

    attachInterrupt(
            digitalPinToInterrupt(ENCODER_LEFT_PIN),
            encoderTick,
            CHANGE
    );
    attachInterrupt(
            digitalPinToInterrupt(ENCODER_RIGHT_PIN),
            encoderTick,
            CHANGE
    );

    renderRXUI();
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
        frequency->loop();
    } // else

    {

        auto currentMS = millis();
        // save cycles, we don't have to evaluate all that stuff with 16MHz frequency
        if (currentMS - loopMS > 0) {

            freqEncButton->loop();
            modeButton->loop();
            vfoButton->loop();
            stepButton->loop();
            menuButton->loop();
            bandButton->loop();
            freqEncoder.tick();

            int32_t lastestEncoderPosition = freqEncoder.getPosition();
            if (lastestEncoderPosition != encoderPosition && freqEncButton->isPressed()) {
                freqEncButton->disable();
            }
            auto delta = lastestEncoderPosition - encoderPosition;
            if (delta != 0) {
                unsigned long deltaT = freqEncoder.getMillisBetweenRotations();
                if (deltaT > 1) {
                    if (deltaT > 500) {
                        deltaT = 500;
                    }
                } else {
                    deltaT = 2;
                }
                float deltaf = delta * (6.2 / log(deltaT));
                delta = long(deltaf);
            }

            // turning the knob counter-clockwise
            if (lastestEncoderPosition > encoderPosition) {
                if (currentMenu->isActive()) {
                    currentMenu->down();
                } else if (subMenuScreen != MsgExit) {
                    encoderCCW();
                } else if (freqEncButton->isPressed()) {
                    frequency->changeFrequencyStep(-1);
                } else if (state.isRIT) {
                    if (state.RITFrequency > -9999) {
                        state.RITFrequency += delta;
                        frequency->updatePllFrequency();
                    }
                    if (!currentMenu->isActive()) {
                        frequency->scheduleRedraw(Data);
                    }
                } else {
                    state.frequency += state.step * delta;
                    band->loop();
                    frequency->loop();
                }
                encoderPosition = lastestEncoderPosition;
            } else if (lastestEncoderPosition < encoderPosition) {
                // turning the knob clockwise
                if (currentMenu->isActive()) {
                    currentMenu->up();
                } else if (subMenuScreen != MsgExit) {
                    encoderCW();
                } else if (freqEncButton->isPressed()) {
                    frequency->changeFrequencyStep(1);
                } else if (state.isRIT) {
                    if (state.RITFrequency < 9999) {
                        state.RITFrequency += delta;
                        frequency->updatePllFrequency();
                    }
                    if (!currentMenu->isActive()) {
                        frequency->scheduleRedraw(Data);
                    }
                } else {
                    state.frequency += state.step * delta;
                    band->loop();
                    frequency->loop();
                }
                encoderPosition = lastestEncoderPosition;
            }

            if (state.frequency > UPPER_RX_BOUND) {
                state.frequency = LOWER_RX_BOUND;
            } else if (state.frequency < LOWER_RX_BOUND) {
                state.frequency = UPPER_RX_BOUND;
            }

            if (!currentMenu->isActive()) {
                yield();
                band->loop();
            }

            loopMS = currentMS;
            // settle
            yield();
        }
        if (!currentMenu->isActive() && subMenuScreen == MsgExit) {
            sMeter->loop();
            pano->loop();
            band->loop();
            frequency->loop();
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

