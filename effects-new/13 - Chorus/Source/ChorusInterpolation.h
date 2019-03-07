#pragma once
#include "JuceHeader.h"

/*  ChorusInterpolation: Encapsulates the basic interpolation functions of the Chorus effect.
 */

struct ChorusInterpolation
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
