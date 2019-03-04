#include "Distortion.h"

void Distortion::populateDistortionTypeComboBox(ComboBox& cb)
{
    cb.clear(dontSendNotification);
    int itemNumber = 0;
    cb.addItem("Hard Clipping", ++itemNumber);
    cb.addItem("Soft Clipping", ++itemNumber);
    cb.addItem("Exp. Soft Clip", ++itemNumber);
    cb.addItem("Full-Wave Rectifier", ++itemNumber);
    cb.addItem("Half-Wave Rectifier", ++itemNumber);
}

float Distortion::processSample(float sample, Type type)
{
    switch (type)
    {
    case kHardClipping:
    {
        const float threshold = 0.5f;
        if (sample > threshold) sample = threshold;         // positive clip
        else if (sample < -threshold) sample = -threshold;  // negative clip
        break;
    }

    case kSoftClipping:
    {
        const float threshold1 = 1.0f / 3.0f;
        const float threshold2 = 2.0f / 3.0f;

        if (sample > threshold1)
        {
            if (sample > threshold2) sample = 0.5f;         // positive clip
            else // soft knee (positive)
                sample = (3.0f - (2.0f - 3.0f * sample) * (2.0f - 3.0f * sample)) / 6.0f;
        }
        else if (sample < -threshold1)
        {
            if (sample < -threshold2) sample = -0.5f;       // negative clip
            else // soft knee (negative)
                sample = -(3.0f - (2.0f + 3.0f * sample) * (2.0f + 3.0f * sample)) / 6.0f;
        }
        break;
    }

    case kSoftClippingExp:
    {
        if (sample > 0.0f)  // positive
            sample = 1.0f - expf(-sample);
        else                // negative
            sample = -1.0f + expf(sample);
        break;
    }

    case kFullWaveRectifier:
    {
        sample = fabs(sample);
        break;
    }

    case kHalfWaveRectifier:
    {
        sample = 0.5f * (fabs(sample) + sample);
        break;
    }

    default:
        break;
    }

    return sample;
}
