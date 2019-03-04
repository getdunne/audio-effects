#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class ParametricEQParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI
    // Labels are supplementary, typically used for units of measure
    static const String centreFreqID, centreFreqName, centreFreqLabel;
    static const String filterQID, filterQName, filterQLabel;
    static const String gainID, gainName, gainLabel;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    ParametricEQParameters(AudioProcessorValueTreeState& vts,
                           AudioProcessorValueTreeState::Listener* processor);
    ~ParametricEQParameters();

    void detachControls();
    void attachControls(
        Slider& centreFreqKnob,
        Slider& filterQKnob,
        Slider& gainKnob );

    // working parameter values
    float centreFreqHz;
    float filterQ;
    float linearGain;

    // derived value
    float getBandwidthHz() { return centreFreqHz / filterQ; }

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> centreFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> filterQAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    // Listener objects link parameters to working variables
    FloatListener centreFreqListener;
    FloatListener filterQListener;
    FloatDecibelListener gainListener;
};