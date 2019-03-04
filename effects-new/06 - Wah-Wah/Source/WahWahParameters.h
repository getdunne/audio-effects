#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class WahWahParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI
    // Labels are supplementary, typically used for units of measure
    static const String centreFreqID, centreFreqName, centreFreqLabel;
    static const String filterQID, filterQName, filterQLabel;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    WahWahParameters(AudioProcessorValueTreeState& vts,
                     AudioProcessorValueTreeState::Listener* processor);
    ~WahWahParameters();

    void detachControls();
    void attachControls(
        Slider& centreFreqKnob,
        Slider& filterQKnob );

    // working parameter values
    float centreFreqHz;
    float filterQ;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> centreFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> filterQAttachment;

    // Listener objects link parameters to working variables
    FloatListener centreFreqListener;
    FloatListener filterQListener;
};