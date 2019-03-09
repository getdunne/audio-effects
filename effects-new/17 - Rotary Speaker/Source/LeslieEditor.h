#pragma once
#include "JuceHeader.h"
#include "LeslieProcessor.h"
#include "LabeledKnob.h"

class LeslieEditor  : public AudioProcessorEditor
{
public:
    LeslieEditor (LeslieProcessor&);
    virtual ~LeslieEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    LeslieProcessor& processor;

    GroupComponent mainGroup;

    BasicKnob speedKnob; LabeledKnob labeledSpeedKnob;
    ToggleButton midiSustainToggle, midiModWheelToggle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LeslieEditor)
};