#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class AutoWahParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI
    // Labels are supplementary, typically used for units of measure
    static const String centreFreqID, centreFreqName, centreFreqLabel;
    static const String filterQID, filterQName, filterQLabel;
    static const String lfoFreqID, lfoFreqName, lfoFreqLabel;
    static const String lfoWidthID, lfoWidthName, lfoWidthLabel;
    static const String attackID, attackName, attackLabel;
    static const String decayID, decayName, decayLabel;
    static const String envWidthID, envWidthName, envWidthLabel;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    AutoWahParameters(AudioProcessorValueTreeState& vts,
                      AudioProcessorValueTreeState::Listener* processor);
    ~AutoWahParameters();

    void detachControls();
    void attachControls(
        Slider& centreFreqKnob,
        Slider& filterQKnob,
        Slider& lfoFreqKnob,
        Slider& lfoWidthKnob,
        Slider& attackKnob,
        Slider& decayKnob,
        Slider& envWidthKnob );

    // working parameter values
    float centreFreqHz;
    float filterQ;
    float lfoFreqHz;
    float lfoWidthHz;
    float attackTimeSec;
    float decayTimeSec;
    float envWidthHz;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> centreFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> filterQAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoWidthAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> envWidthAttachment;

    // Listener objects link parameters to working variables
    FloatListener centreFreqListener;
    FloatListener filterQListener;
    FloatListener lfoFreqListener;
    FloatListener lfoWidthListener;
    FloatListener attackListener;
    FloatListener releaseListener;
    FloatListener envWidthListener;
};