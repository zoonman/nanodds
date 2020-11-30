//
// Created by Philipp Tkachev on 2019-03-09.
//

#ifndef NANODDS_MODE_H
#define NANODDS_MODE_H

#pragma pack(push)
enum Mode {
    CW = 0, LSB, USB, AM, FM
};
const static char *ModeNames[] = {"CW", "LSB", "USB", "AM", "FM"};
#pragma pack(pop)


enum VFOType {
    VFO_A = 0, VFO_B, VFO_SPLIT
};

enum Message {
    MsgMenu = 0,
    MsgMemory,
    MsgAbout,
    MsgCW,
    MsgSWR,
    MsgIF,
    MsgSSBOffset,
    MsgDDSCalibration,
    MsgSWRCalibration,
    MsgSettings,
    MsgExit,
    MsgSSB,
    MsgSaveToNewCell,
    MsgErase,
    MsgLoadFromTheCell,
    MsgErasing,
    MsgAir,
    MsgTogglePano,
    MsgSSB85,
    MsgSaving,
    MsgInvalidCRC,
    MsgOK,
    MsgRX,
    MsgTX,
    MsgVFOA,
    MsgVFOB,
    MsgVFOSPLIT,
};

const static char Messages[] PROGMEM = {
        "Menu"
        "Memory"
        "About"
        "CW"
        "SWR"
        "Intermediate Frequency"
        "SSB Offset"
        "DDS Calibration"
        "SWR Calibration"
        "Settings"
        "Exit"
        "SSB"
        "Save To New Cell"
        "Erase"
        "Load from the Cell"
        "Erasing..."
        "AIR"
        "Pano on/off"
        "SSB85 Transceiver"
        "Saving..."
        "Invalid CRC"
        "OK"
        "RX"
        "TX"
        "VFO A"
        "VFO B"
        "SPLIT"
};

const static uint8_t MessageLengths[] PROGMEM = {
        4,
        6,
        5,
        2,
        3,
        22,
        10,
        15,
        15,
        8,
        4,
        3,
        16,
        5,
        18,
        10,
        3,
        11,
        17,
        9,
        11,
        2,
        2,
        2,
        5,
        5,
        5
};

#endif //NANODDS_MODE_H
