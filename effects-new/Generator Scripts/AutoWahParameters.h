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
    static const String lfoDepthID, lfoDepthName, lfoDepthLabel;
    static const String attackID, attackName, attackLabel;
    static const String releaseID, releaseName, releaseLabel;
    static const String modDepthID, modDepthName, modDepthLabel;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    AutoWahParameters(AudioProcessorValueTreeState& vts);
    ~AutoWahParameters();

    void detachControls();
    void attachControls(
        Slider& centreFreqKnob,
        Slider& filterQKnob,
        Slider& lfoFreqKnob,
        Slider& lfoDepthKnob,
        Slider& attackKnob,
        Slider& releaseKnob,
        Slider& modDepthKnob );

    // working parameter values
    float centreFreqHz;
    float filterQ;
    float lfoFreqHz;
    float lfoDepth;
    float attackTimeMs;
    float releaseTimeMs;
    float envDepth;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> centreFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> filterQAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoDepthAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> modDepthAttachment;

    // Listener objects link parameters to working variables
    FloatListener centreFreqListener;
    FloatListener filterQListener;
    FloatListener lfoFreqListener;
    FloatListener lfoDepthListener;
    FloatListener attackListener;
    FloatListener releaseListener;
    FloatListener modDepthListener;
};