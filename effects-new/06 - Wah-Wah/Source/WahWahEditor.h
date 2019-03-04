#pragma once
#include "JuceHeader.h"
#include "WahWahProcessor.h"
#include "LabeledKnob.h"

class WahWahEditor  : public AudioProcessorEditor
{
public:
    WahWahEditor (WahWahProcessor&);
    virtual ~WahWahEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    WahWahProcessor& processor;

    GroupComponent mainGroup;

    BasicKnob centreFreqKnob; LabeledKnob labeledCentreFreqKnob;
    BasicKnob filterQKnob; LabeledKnob labeledFilterQKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WahWahEditor)
};