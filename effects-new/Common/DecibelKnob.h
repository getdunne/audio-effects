#pragma once
#include "JuceHeader.h"
#include "BasicKnob.h"

/*  DecibelKnob is a specialization of BasicKnob, for decibel-valued knobs.
    Adapted from JUCE Tutorial: Control audio levels using decibels
    https://docs.juce.com/master/tutorial_synth_db_level_control.html
*/

class DecibelKnob : public BasicKnob
{
public:
    DecibelKnob(float minDB, float maxDB, float minusInfDB=-100.0f);
    virtual ~DecibelKnob() = default;

    // Slider
    double getValueFromText(const String& text) override;
    String getTextFromValue(double value) override;

private:
    const float minusInfinitydB;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DecibelKnob)
};
