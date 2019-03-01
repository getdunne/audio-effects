#pragma once
#include "JuceHeader.h"
#include "GainProcessor.h"
#include "LabeledKnob.h"
#include "DecibelKnob.h"

class GainEditor  : public AudioProcessorEditor, protected Timer
{
public:
    GainEditor (GainProcessor&);
    virtual ~GainEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

protected:
    void timerCallback() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    GainProcessor& processor;

    GroupComponent undoGroup;
    TextButton undoButton, redoButton;

    GroupComponent gainGroup;
    DecibelKnob gainKnob;
    LabeledKnob labeledGainKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainEditor)
};
