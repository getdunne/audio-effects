#pragma once
#include "JuceHeader.h"
#include "DistortionProcessor.h"
#include "LabeledKnob.h"
#include "DecibelKnob.h"

class DistortionEditor  : public AudioProcessorEditor
{
public:
    DistortionEditor (DistortionProcessor&);
    virtual ~DistortionEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    DistortionProcessor& processor;

    GroupComponent mainGroup;

    ComboBox distTypeCombo; Label distTypeLabel;
    DecibelKnob gainKnob; LabeledKnob labeledGainKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionEditor)
};
