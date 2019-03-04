#pragma once
#include "JuceHeader.h"

/*  Vibrato: Encapsulates the basic wave-shaping interpolation functions of the Vibrato effect.
 */

struct Vibrato
{
    enum Type
    {
        kInterpolationNearestNeighbour = 1,
        kInterpolationLinear,
        kInterpolationCubic,
        kNumInterpolations
    };
    
    static void populateInterpolationComboBox(ComboBox& cb);
    
    static float processSample(float sample, Type type);
};
