#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class DelayParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String delayTimeID, delayTimeName, delayTimeLabel;
    static const float delayTimeMin, delayTimeMax, delayTimeDefault, delayTimeStep;
    static const String feedbackID, feedbackName, feedbackLabel;
    static const float feedbackMin, feedbackMax, feedbackDefault, feedbackStep;
    static const String dryLevelID, dryLevelName, dryLevelLabel;
    static const float dryLevelMin, dryLevelMax, dryLevelDefault, dryLevelStep;
    static const String wetLevelID, wetLevelName, wetLevelLabel;
    static const float wetLevelMin, wetLevelMax, wetLevelDefault, wetLevelStep;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    DelayParameters(AudioProcessorValueTreeState& vts,
                    AudioProcessorValueTreeState::Listener* processor);
    ~DelayParameters();

    void detachControls();
    void attachControls(
        Slider& delayTimeKnob,
        Slider& feedbackKnob,
        Slider& dryLevelKnob,
        Slider& wetLevelKnob );

    // working parameter values
    float delaySec;
    float feedback;
    float dryLevel;
    float wetLevel;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dryLevelAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> wetLevelAttachment;

    // Listener objects link parameters to working variables
    FloatListener delayTimeListener;
    FloatListener feedbackListener;
    FloatListener dryLevelListener;
    FloatListener wetLevelListener;
};