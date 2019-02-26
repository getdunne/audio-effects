#pragma once
#include "JuceHeader.h"

struct GainParameters
{
    // Id's are symbolic names, Names are human-friendly names for GUI
    // Labels are supplementary, typically used for units of measure
    static const String gainID, gainName, gainLabel;

    GainParameters(AudioProcessorValueTreeState& vts);
    void createAllParameters();
    void detachControls();
    void attachControls(Slider& levelSlider);

    // Actual working parameter values
    float gain;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Attachment objects link GUI controls to parameters
    using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
    SliderAttachment* pGainAttachment;

    // Specialized versions of AudioProcessorValueTreeState::Listener adapted for our parameter types
    struct FloatListener : public AudioProcessorValueTreeState::Listener
    {
        float& workingValue;
        float scaleFactor;      // multiply parameter values by this to get working value

        FloatListener(float& wv, float sf=1.0f) : AudioProcessorValueTreeState::Listener(), workingValue(wv), scaleFactor(sf) {}
        void parameterChanged(const String&, float newValue) override
        {
            workingValue = scaleFactor * newValue;
        }
    };

    FloatListener gainListener;
};
