#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class PingPongDelayParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String delayTimeLID, delayTimeLName, delayTimeLLabel;
    static const float delayTimeLMin, delayTimeLMax, delayTimeLDefault, delayTimeLStep;
    static const String delayTimeRID, delayTimeRName, delayTimeRLabel;
    static const float delayTimeRMin, delayTimeRMax, delayTimeRDefault, delayTimeRStep;
    static const String feedbackID, feedbackName, feedbackLabel;
    static const float feedbackMin, feedbackMax, feedbackDefault, feedbackStep;
    static const String wetLevelID, wetLevelName, wetLevelLabel;
    static const float wetLevelMin, wetLevelMax, wetLevelDefault, wetLevelStep;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    PingPongDelayParameters(AudioProcessorValueTreeState& vts,
                            AudioProcessorValueTreeState::Listener* processor);
    ~PingPongDelayParameters();

    void detachControls();
    void attachControls(
        Slider& delayTimeLKnob,
        Slider& delayTimeRKnob,
        Slider& feedbackKnob,
        Slider& wetLevelKnob );

    // working parameter values
    float delaySecL;
    float delaySecR;
    float feedback;
    float wetLevel;
    bool reverseChannels;
    bool linkDelays;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delayTimeLAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delayTimeRAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> wetLevelAttachment;

    // Listener objects link parameters to working variables
    FloatListener delayTimeLListener;
    FloatListener delayTimeRListener;
    FloatListener feedbackListener;
    FloatListener wetLevelListener;
};