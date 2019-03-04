#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"
#include "Distortion.h"

class DistortionParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI
    // Labels are supplementary, typically used for units of measure
    static const String distTypeID, distTypeName, distTypeLabel;
    static const String gainID, gainName, gainLabel;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    DistortionParameters(AudioProcessorValueTreeState& vts);
    ~DistortionParameters();

    void detachControls();
    void attachControls(ComboBox& distTypeCombo, Slider& gainKnob);

    // working parameter values
    Distortion::Type distType;
    float linearGain;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> distTypeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    // Listener objects link parameters to working variables
    EnumListener<Distortion::Type> distTypeListener;
    FloatDecibelListener gainListener;  // gainID -> linearGain
};
