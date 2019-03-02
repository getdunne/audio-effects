#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class GainParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI
    // Labels are supplementary, typically used for units of measure
    static const String gainID, gainName, gainLabel;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    GainParameters(AudioProcessorValueTreeState& vts);
    ~GainParameters();

    void detachControls();
    void attachControls(Slider& gainKnob);

    // linearGain is used by our processor, converted as needed by our Listener
    float linearGain;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    // Listener objects link parameters to working variables
    FloatDecibelListener gainListener;  // gainID -> linearGain
};
