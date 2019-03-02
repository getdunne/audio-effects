#pragma once
#include "JuceHeader.h"

/*  LFO type 1: a simple biased-output (range [0, 1]) LFO, used in the Tremolo effect.
*/

struct LFO_1
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
