#pragma once
#include "JuceHeader.h"

/*  VibratoInterpolation: Encapsulates the basic interpolation functions of the Vibrato effect.
 */

struct VibratoInterpolation
{
    enum Type
    {
        kInterpolationNearestNeighbour = 0,
        kInterpolationLinear,
        kInterpolationCubic,
        kNumInterpolations
    };

    static void populateInterpolationComboBox(ComboBox& cb);

    static float processSample(Type type, float delayReadPosition,
                               float* delayData, int delayBufferLength);
};
