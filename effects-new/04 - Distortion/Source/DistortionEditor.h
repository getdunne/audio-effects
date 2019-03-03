#pragma once
#include "JuceHeader.h"
#include "DistortionProcessor.h"
#include "LabeledKnob.h"
#include "DecibelKnob.h"

class DistortionEditor  : public AudioProcessorEditor, protected Timer
{
public:
    DistortionEditor (DistortionProcessor&);
    virtual ~DistortionEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

protected:
    void timerCallback() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    DistortionProcessor& processor;

    GroupComponent undoGroup;
    TextButton undoButton, redoButton;

    GroupComponent gainGroup;
    DecibelKnob gainKnob;
    LabeledKnob labeledGainKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionEditor)
};
