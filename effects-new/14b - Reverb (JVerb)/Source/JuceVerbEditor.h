#pragma once
#include "JuceHeader.h"
#include "JuceVerbProcessor.h"
#include "LabeledKnob.h"

class JuceVerbEditor  : public AudioProcessorEditor
{
public:
    JuceVerbEditor (JuceVerbProcessor&);
    virtual ~JuceVerbEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    JuceVerbProcessor& processor;

    GroupComponent mainGroup;

    BasicKnob roomSizeKnob; LabeledKnob labeledRoomSizeKnob;
    BasicKnob dampingKnob; LabeledKnob labeledDampingKnob;
    BasicKnob wetLevelKnob; LabeledKnob labeledWetLevelKnob;
    BasicKnob dryLevelKnob; LabeledKnob labeledDryLevelKnob;
    BasicKnob widthKnob; LabeledKnob labeledWidthKnob;
    ToggleButton freezeModeToggle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceVerbEditor)
};