
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

        for (uint8_t x = 0; x < TFT_WIDTH; x++ ) {
            tft.drawFastVLine(x & 0xFE, SWR_SCALE_Y, 5, COLOR_GRAY_MEDIUM);
        }
    }

    void clear() {

    }

    void loop() {
        // measure voltages
        int ref = analogRead(this->pinReflected);
        int fwd = analogRead(this->pinForward);
        if (fwd == 0) {
            fwd = 1;
        }
        // todo: optimize swr calculation
        // ref is multiplied by 10
        auto measuredSwr = static_cast<uint8_t >((ref * 10 / fwd) & 0xFF);
        measuredSwr = measuredSwr > 10 ? measuredSwr : (uint8_t)10;
        if (this->swr != measuredSwr) {
            state.swr = measuredSwr;
            uint8_t mSwrX = min(this->convertSWR(measuredSwr), TFT_WIDTH);
            uint8_t cSwrX = min(this->convertSWR(this->swr), TFT_WIDTH);

            if (mSwrX > cSwrX) {
                for (uint8_t x = cSwrX; x < mSwrX; x+=2) {
                    char swrb[8] = "\0";

                    intToStrFP(swrb, state.swr, 1, 5);
                    textxy(
                        20,
                        SWR_SCALE_TY,
                        swrb,
                        swrToColor(state.swr),
                        ST77XX_BLACK
                    );

                }
            } else {
                for (uint8_t x = cSwrX; x > mSwrX; x-=2) {
                    tft.drawFastVLine(x & 0xFE, SWR_SCALE_Y, 5, COLOR_GRAY_MEDIUM);
                }
            }

            this->swr = measuredSwr;
        }
    }

    uint16_t swrToColor(uint8_t swr) {
        if (swr > 30 ) {
            return COLOR_BRIGHT_RED;
        }
        if (swr > 20) {
            return ST77XX_YELLOW;
        }
        if (swr > 15) {
            return COLOR_BRIGHT_GREEN;
        }
        return ST77XX_GREEN;
    }

    uint16_t renderSwr(uint8_t x) {
        uint16_t color = COLOR_BRIGHT_GREEN;
        if (x < this->convertSWR(15)) {
            color = COLOR_BRIGHT_GREEN;
        } else if (x < this->convertSWR(30)) {
            color = ST77XX_YELLOW;
        } else {
            color = COLOR_BRIGHT_RED;
        }
        tft.drawFastVLine(x & 0xFE, SWR_SCALE_Y, 5, color);
        return color;
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
        tft.drawFastVLine(swrx, SWR_SCALE_Y, 5, ST77XX_WHITE);
    }
private:
    volatile uint8_t pl = 1;
    volatile uint8_t swr = 1;
    int pd = 0;
    uint8_t pinReflected;
    uint8_t pinForward;
    const uint8_t delta = 1;
};

#endif //NANODDS_SWR_H
