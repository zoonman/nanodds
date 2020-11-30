//
// Created by Philipp Tkachev on 2019-10-28.
//

#include "AppSettings.h"

AppSettings CloneSettings(AppSettings  const& o) {
    AppSettings r;
    r.iFrequency = o.iFrequency;
    r.ssbOffset = o.ssbOffset;
    r.cwOffset = o.cwOffset;
    r.wpm = o.wpm;
    r.isPanoEnabled = o.isPanoEnabled;
    return r;
}

AppSettings SetAppSettings(AppSettings  & r, AppSettings o) {
    r.iFrequency = o.iFrequency;
    r.ssbOffset = o.ssbOffset;
    r.cwOffset = o.cwOffset;
    r.wpm = o.wpm;
    r.isPanoEnabled = o.isPanoEnabled;
}


