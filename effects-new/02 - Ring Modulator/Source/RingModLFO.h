#pragma once
#include "JuceHeader.h"

/*  A simple symmetric-output (range [-1, 1]) LFO, used in the Ring Modulator effect.
*/

struct RingModLFO
{
    enum Waveform
    {
        kWaveformSine = 0,
        kWaveformTriangle,
        kWaveformSquare,
        kWaveformSawtooth,
        kWaveformInverseSawtooth,
        kNumWaveforms
    };

    static void populateWaveformComboBox(ComboBox& cb);

    static float getSample(float phase, Waveform waveform);
};
