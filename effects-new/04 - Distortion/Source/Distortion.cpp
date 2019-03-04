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

float Distortion::processSample(float inputSample, float linearGain, Type type)
{
    switch (type)
    {
    case kHardClipping:
        return linearGain * inputSample;

    case kSoftClipping:
        return linearGain * inputSample;

    case kSoftClippingExp:
        return linearGain * inputSample;

    case kFullWaveRectifier:
        return linearGain * inputSample;

    case kHalfWaveRectifier:
        return linearGain * inputSample;

    default:
        return linearGain * inputSample;
    }
}
