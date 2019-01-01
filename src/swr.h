
#ifndef NANODDS_SWR_H
#define NANODDS_SWR_H

#include "screen.h"

class SWRMeter {
public:
    explicit SWRMeter(uint8_t pinReflected, uint8_t pinForward) {
        this->pinReflected = pinReflected;
        this->pinForward = pinForward;
    };

    void setup() {
        // add pin listener

        // render scale
        render();
    }

    void render() {

        tft.drawFastVLine(0, SWR_SCALE_Y, 5, COLOR_GRAY_MEDIUM);
        tft.fillRect(0, SWR_SCALE_Y, TFT_WIDTH-1, 5, tft.color565(20, 20, 20));

        textxy(0, SWR_SCALE_TY, ("SWR"), COLOR_GRAY_MEDIUM, ST77XX_BLACK);
        displaySWRTick(15, "1.5");
        displaySWRTick(20, "2");
        displaySWRTick(30, "3");
        displaySWRTick(50, "5");
        displaySWRTick(100, "10");
        tft.drawFastVLine(0, SWR_SCALE_Y, 6, COLOR_GRAY_MEDIUM);
        textxy(TFT_WIDTH - 20, SWR_SCALE_TY, ("inf"));
    }

#define DELTA 1
    void displaySWR() {
        // 1..1.5..2..3...inf
        uint8_t swr = (uint8_t)(analogRead(A1) / 4); // 10 = 1, 15 = 1.5, 20 = 2, etc, max is 255, 1024
        /**
         * SWR is calculated as ratio REFLECTED/DIRECT POWER
         */
        swr = swr > 10 ? swr : (uint8_t)10;

        if (swr > state.swr + DELTA || swr < state.swr - DELTA) {
            state.swr = swr;
            tft.drawFastVLine(0, SWR_SCALE_Y, 5, COLOR_GRAY_MEDIUM);
            tft.fillRect(0, SWR_SCALE_Y, TFT_WIDTH-1, 5, tft.color565(20, 20, 20));

            uint8_t swrX = convertSWR(state.swr);
            uint16_t color = COLOR_BRIGHT_GREEN;
            for (uint8_t x = 0; x < min(swrX, TFT_WIDTH); x+=2) {
                if (x < convertSWR(15)) {
                    color = COLOR_BRIGHT_GREEN;
                } else if (x < convertSWR(30)) {
                    color = ST77XX_YELLOW;
                } else {
                    color = COLOR_BRIGHT_RED;
                }
                tft.drawFastVLine(x, SWR_SCALE_Y, 5, color);
            }
            textxy(0, SWR_SCALE_TY, ("SWR"), COLOR_GRAY_MEDIUM, ST77XX_BLACK);
            char swrb[8] = "\0";
            // utoa(state.swr, b, 10);
            intToStrFP(swrb, state.swr, 1, 5);
            textxy(20, SWR_SCALE_TY, swrb, color, ST77XX_BLACK);
            displaySWRTick(15, "1.5");
            displaySWRTick(20, "2");
            displaySWRTick(30, "3");
            displaySWRTick(50, "5");
            displaySWRTick(100, "10");
            tft.drawFastVLine(0, SWR_SCALE_Y, 6, COLOR_GRAY_MEDIUM);
            textxy(TFT_WIDTH - 20, SWR_SCALE_TY, ("inf"));
        }
    }


    void loop() {
        int ref = analogRead(this->pinReflected);
        int fwd = analogRead(this->pinForward);

    }

    uint8_t convertSWR(uint8_t swr) {
        return TFT_WIDTH - static_cast<uint8_t>(TFT_WIDTH/log(swr-10));
    }

    void displaySWRTick(uint8_t value, const char label[]) {
        uint8_t swrx = convertSWR(value);
        if (swrx % 2 == 0) {
            swrx++;
        }
        textxy(swrx - strlen(label) * 5 / 2, SWR_SCALE_TY, label, COLOR_GRAY_MEDIUM, ST77XX_BLACK);
        tft.drawFastVLine(swrx, SWR_SCALE_Y, 5, COLOR_GRAY_MEDIUM);
    }
private:
    volatile uint8_t pl = 1;
    int pd = 0;
    uint8_t pinReflected;
    uint8_t pinForward;

}

#endif //NANODDS_SWR_H
