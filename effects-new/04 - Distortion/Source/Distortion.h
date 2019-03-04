#pragma once
#include "JuceHeader.h"

/*  Distortion: Encapsulates the basic wave-shaping functions of the Distortion effect.
*/

struct Distortion
{
    enum Type
    {
        kHardClipping = 0,
        kSoftClipping,
        kSoftClippingExp,
        kFullWaveRectifier,
        kHalfWaveRectifier,
        kNumTypes
    };

    static void populateDistortionTypeComboBox(ComboBox& cb);

    static float processSample(float sample, Type type);
};
