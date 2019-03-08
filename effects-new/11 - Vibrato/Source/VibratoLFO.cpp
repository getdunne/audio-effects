#include "VibratoLFO.h"
#include <cmath>
#define TWOPI_F 6.283185f

void VibratoLFO::populateWaveformComboBox(ComboBox& cb)
{
    cb.clear(dontSendNotification);
    int itemNumber = 0;
    cb.addItem("Sine", ++itemNumber);
    cb.addItem("Triangle", ++itemNumber);
    cb.addItem("Sawtooth", ++itemNumber);
    cb.addItem("Inverse Sawtooth", ++itemNumber);
}

// Function for calculating "biased" LFO waveforms with output range [0, 1].
// Phase range [0, 1], output also [0, 1] (not [-1, +1] as for the ordinary Sine function).
float VibratoLFO::getSample(float phase, Waveform waveform)
{
    switch (waveform)
    {
    case kWaveformTriangle:
        if (phase < 0.25f)
            return 0.5f + 2.0f*phase;
        else if (phase < 0.75f)
            return 1.0f - 2.0f*(phase - 0.25f);
        else
            return 2.0f*(phase - 0.75f);
    case kWaveformSawtooth:
        if (phase < 0.5f)
            return 0.5f + phase;
        else
            return phase - 0.5f;
    case kWaveformInverseSawtooth:
        if (phase < 0.5f)
            return 0.5f - phase;
        else
            return 1.5f - phase;
    case kWaveformSine:
    default:
        return 0.5f + 0.5f*sinf(TWOPI_F * phase);
    }
}
