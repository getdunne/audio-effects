#include "RingModLFO.h"
#include <cmath>
#define TWOPI_F 6.283185f

void RingModLFO::populateWaveformComboBox(ComboBox& cb)
{
    cb.clear(dontSendNotification);
    int itemNumber = 0;
    cb.addItem("Sine", ++itemNumber);
    cb.addItem("Triangle", ++itemNumber);
    cb.addItem("Square", ++itemNumber);
    cb.addItem("Sawtooth", ++itemNumber);
    cb.addItem("Inverse Sawtooth", ++itemNumber);
}

// Function for calculating "unbiased" LFO waveforms with output range [-1, +1].
// Phase range [0, 1], output [-1, +1] (no DC bias).
float RingModLFO::getSample(float phase, Waveform waveform)
{
    switch (waveform)
    {
    case kWaveformTriangle:
        if (phase < 0.25f)
            return 4.0f*phase;
        else if (phase < 0.75f)
            return 1.0f - 4.0f*(phase - 0.25f);
        else
            return -1.0f + 4.0f*(phase - 0.75f);
    case kWaveformSquare:
        if (phase < 0.5f)
            return 1.0f;
        else
            return -1.0f;
    case kWaveformSawtooth:
        if (phase < 0.5f)
            return 2.0f*phase;
        else
            return 2.0f*phase - 2.0f;
    case kWaveformInverseSawtooth:
        if (phase < 0.5f)
            return -2.0f*phase;
        else
            return 2.0f - 2.0f*phase;
    case kWaveformSine:
    default:
        return sinf(TWOPI_F * phase);
    }
}
