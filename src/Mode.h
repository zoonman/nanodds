//
// Created by Philipp Tkachev on 2019-03-09.
//

#ifndef NANODDS_MODE_H
#define NANODDS_MODE_H

#pragma pack(push)
enum Mode {
    CW = 0, LSB, USB, AM, FM
};
static const char *ModeNames[] = {"CW", "LSB", "USB", "AM", "FM"};
#pragma pack(pop)


#endif //NANODDS_MODE_H
