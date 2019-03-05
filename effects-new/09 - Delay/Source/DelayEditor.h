#pragma once
#include "JuceHeader.h"
#include "DelayProcessor.h"
#include "LabeledKnob.h"

class DelayEditor  : public AudioProcessorEditor
{
public:
    DelayEditor (DelayProcessor&);
    virtual ~DelayEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    DelayProcessor& processor;

    GroupComponent mainGroup;

    BasicKnob delayTimeKnob; LabeledKnob labeledDelayTimeKnob;
    BasicKnob feedbackKnob; LabeledKnob labeledFeedbackKnob;
    BasicKnob dryLevelKnob; LabeledKnob labeledDryLevelKnob;
    BasicKnob wetLevelKnob; LabeledKnob labeledWetLevelKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayEditor)
};