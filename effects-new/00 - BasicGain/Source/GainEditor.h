#pragma once
#include "JuceHeader.h"
#include "GainProcessor.h"
#include "CustomControls.h"

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
    DecibelGainKnob gainKnob;
    Label gainLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainEditor)
};
