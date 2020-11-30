//
// Created by Philipp Tkachev on 2019-10-28.
//

#ifndef NANODDS_APPSETTINGS_H
#define NANODDS_APPSETTINGS_H

#include <stdint.h>

struct AppSettings {
    /**
     * Intermediate frequency
     * */
    //uint32_t iFrequency = 455000;// max:-5.69dB 7.998928MHz
    uint32_t iFrequency = 7998500;// max:-5.69dB 7.998928MHz
    /**
     * SSB offset
     */
    int32_t ssbOffset = 1500;
    /**
     * CW Offset
     */
    uint32_t cwOffset = 800;

    /**
     * CW Words per minute
     *
     * The basic element of Morse code is the dot and all other elements
     * can be defined in terms of multiples of the dot length.
     * The word PARIS is used because this is the length of a typical word
     * in English plain text, it has a total length of 50 dot lengths.
     * If the word PARIS can be sent ten times in a minute using
     * normal Morse code timing then the code speed is 10 WPM.
     *
     * dot_ms = 60 * 1000 / (wpm * 50) = 1200 / wpm
     */
    uint8_t wpm = 10;

    /**
     * Enable or disable panoramic view
     * */
    bool isPanoEnabled = false;

    /**
     * VCO Calibration offset (PPB)
     */
    int64_t pllCalibration = 0;

    /**
     * SWR Calibration
     * S-Meter Calibration
     */
};



struct MemorySettingsCell {
    AppSettings settings;
    uint32_t crc{};
};


#endif //NANODDS_APPSETTINGS_H
