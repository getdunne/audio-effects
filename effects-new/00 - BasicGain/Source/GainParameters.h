#pragma once
#include "JuceHeader.h"

struct GainParameters
{
    // Id's are symbolic names, Names are human-friendly names for GUI
    // Labels are supplementary, typically used for units of measure
    static const String gainID, gainName, gainLabel;

    GainParameters(AudioProcessorValueTreeState& vts);
    ~GainParameters();

    void createAllParameters();
    void detachControls();
    void attachControls(Slider& levelSlider);

    // dbGain is used by the AudioProcessorValueTreeState and Attachments
    // linearGain is used by our processor, converted as needed by our Listener
    float dbGain, linearGain;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Attachment objects link GUI controls to parameters
    using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> gainAttachment;

    // This specialized AudioProcessorValueTreeState::Listener converts decibel gain to linear gain
    struct FloatDecibelListener : public AudioProcessorValueTreeState::Listener
    {
        float& workingValue;
        float minusInfinitydB;

        FloatDecibelListener(float& wv, float minusInfDB)
            : AudioProcessorValueTreeState::Listener()
            , workingValue(wv)
            , minusInfinitydB(minusInfDB)
        {}
        void parameterChanged(const String&, float newValue) override
        {
            workingValue = Decibels::decibelsToGain<float>(newValue, minusInfinitydB);
        }
    };

    FloatDecibelListener gainListener;
};
