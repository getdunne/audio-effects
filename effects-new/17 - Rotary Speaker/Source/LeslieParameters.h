#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class LeslieParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String speedID, speedName, speedLabel;
    static const float speedMin, speedMax, speedDefault, speedStep;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    LeslieParameters(AudioProcessorValueTreeState& vts,
                     AudioProcessorValueTreeState::Listener* processor);
    ~LeslieParameters();

    void detachControls();
    void attachControls(Slider& speedKnob);

    // working parameter values
    float speed;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> speedAttachment;

    // Listener objects link parameters to working variables
    FloatListener speedListener;
};