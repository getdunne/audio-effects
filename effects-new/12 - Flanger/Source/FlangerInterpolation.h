#pragma once
#include "JuceHeader.h"

/*  FlangerInterpolation: Encapsulates the basic interpolation functions of the Flanger effect.
 */

struct FlangerInterpolation
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
