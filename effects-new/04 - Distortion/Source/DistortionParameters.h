#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"
#include "DistortionType.h"

class DistortionParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI
    // Labels are supplementary, typically used for units of measure
    static const String gainID, gainName, gainLabel;
    static const String typeID, typeName, typeLabel;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    DistortionParameters(AudioProcessorValueTreeState& vts);
    ~DistortionParameters();

    void detachControls();
    void attachControls(Slider& gainSlider, ComboBox& typeCombo);

    // linearGain is used by our processor, converted as needed by our Listener
    DistortionType::Type type;
    float linearGain;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> typeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

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
    TypeListener<DistortionType::typeNumber> typeListener;
};
