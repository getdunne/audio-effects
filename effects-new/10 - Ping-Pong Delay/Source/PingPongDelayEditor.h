#pragma once
#include "JuceHeader.h"
#include "PingPongDelayProcessor.h"
#include "LabeledKnob.h"

class PingPongDelayEditor  : public AudioProcessorEditor
{
public:
    PingPongDelayEditor (PingPongDelayProcessor&);
    virtual ~PingPongDelayEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    PingPongDelayProcessor& processor;

    GroupComponent mainGroup;

    BasicKnob delayTimeLKnob; LabeledKnob labeledDelayTimeLKnob;
    BasicKnob delayTimeRKnob; LabeledKnob labeledDelayTimeRKnob;
    BasicKnob feedbackKnob; LabeledKnob labeledFeedbackKnob;
    BasicKnob wetLevelKnob; LabeledKnob labeledWetLevelKnob;
    ToggleButton linkDelaysToggle, reverseChannelsToggle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PingPongDelayEditor)
};