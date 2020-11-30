#include "State.h"

State StateFromVolatile(State volatile const& o) {
    State r;
    r.frequency = o.frequency;
    //r.iFrequency = o.iFrequency;
    //r.ssbOffset = o.ssbOffset;
    //r.cwOffset = o.cwOffset;
    r.altFrequency = o.altFrequency;
    r.step = o.step;
    r.isRIT = o.isRIT;
    r.RITFrequency = o.RITFrequency;
    r.band = o.band;
    r.mode = o.mode;
    r.tx = o.tx;
    r.isAltFrequency = o.isAltFrequency;
    r.wpm = o.wpm;
    r.swr = o.swr;
    r.isPanoEnabled = o.isPanoEnabled;
    return r;
}

State SetVolatileState(State volatile & r, State o) {
    r.frequency = o.frequency;
    // r.iFrequency = o.iFrequency;
    // r.ssbOffset = o.ssbOffset;
    // r.cwOffset = o.cwOffset;
    r.altFrequency = o.altFrequency;
    r.step = o.step;
    r.isRIT = o.isRIT;
    r.RITFrequency = o.RITFrequency;
    r.band = o.band;
    r.mode = o.mode;
    r.tx = o.tx;
    r.isAltFrequency = o.isAltFrequency;
    r.wpm = o.wpm;
    r.swr = o.swr;
    r.isPanoEnabled = o.isPanoEnabled;
}


