#include "TremoloLFO.h"
#include <cmath>
#define TWOPI_F 6.283185f

void TremoloLFO::populateWaveformComboBox(ComboBox& cb)
{
    cb.clear(dontSendNotification);
    int itemNumber = 0;
    cb.addItem("Sine", ++itemNumber);
    cb.addItem("Triangle", ++itemNumber);
    cb.addItem("Square", ++itemNumber);
    cb.addItem("Square (sloped edges)", ++itemNumber);
}

// Function for calculating "biased" LFO waveforms with output range [0, 1].
// Phase range [0, 1], output also [0, 1] (not [-1, +1] as for the ordinary Sine function).
float TremoloLFO::getSample(float phase, Waveform waveform)
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
    case kWaveformSquare:
        if (phase < 0.5f)
            return 1.0f;
        else
            return 0.0f;
    case kWaveformSquareSlopedEdges:
        if (phase < 0.48f)
            return 1.0f;
        else if (phase < 0.5f)
            return 1.0f - 50.0f*(phase - 0.48f);
        else if (phase < 0.98f)
            return 0.0f;
        else
            return 50.0f*(phase - 0.98f);
    case kWaveformSine:
    default:
        return 0.5f + 0.5f*sinf(TWOPI_F * phase);
    }
}
