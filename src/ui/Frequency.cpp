//
// Created by Philipp Tkachev on 2019-10-29.
//

#include "Frequency.h"

void Frequency::loop() {

    if (this->semif != this->state->frequency) {
        if (this->semif > this->state->frequency) {
            this->semif--;
        } else {
            this->semif++;
        }

/*
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
*/
        auto f = this->state->frequency +
                this->settings->iFrequency +
                (this->state->isRIT && !this->state->tx ? this->state->RITFrequency : 0);
        // @todo: add reverse in future
        this->pll->set_freq(
                static_cast<uint64_t>(f) * 100ULL,
                SI5351_CLK0
        );
    }
}



void Frequency::draw() {
    if (!this->isVisible) {
        return;
    }
}