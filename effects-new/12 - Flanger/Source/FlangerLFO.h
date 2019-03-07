#pragma once
#include "JuceHeader.h"

/*  A simple biased-output (range [0, 1]) LFO, used in the Vibrato effect.
*/

struct FlangerLFO
{
    enum Waveform
    {
        kWaveformSine = 0,
        kWaveformTriangle,
        kWaveformSquare,
        kWaveformSawtooth,
        kNumWaveforms
    };

    static void populateWaveformComboBox(ComboBox& cb);

    static float getSample(float phase, Waveform waveform);
};
