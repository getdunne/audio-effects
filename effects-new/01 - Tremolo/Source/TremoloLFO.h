#pragma once
#include "JuceHeader.h"

/*  A simple biased-output (range [0, 1]) LFO, used in the Tremolo effect.
*/

struct TremoloLFO
{
    enum Waveform
    {
        kWaveformSine = 0,
        kWaveformTriangle,
        kWaveformSquare,
        kWaveformSquareSlopedEdges,
        kNumWaveforms
    };

    static void populateWaveformComboBox(ComboBox& cb);

    static float getSample(float phase, Waveform waveform);
};
