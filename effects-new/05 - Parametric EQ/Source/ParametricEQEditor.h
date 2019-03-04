#pragma once
#include "JuceHeader.h"
#include "ParametricEQProcessor.h"
#include "LabeledKnob.h"
#include "DecibelKnob.h"

class ParametricEQEditor : public AudioProcessorEditor
                         , public ChangeListener
{
public:
    ParametricEQEditor (ParametricEQProcessor&);
    virtual ~ParametricEQEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

    // ChangeListener
    void changeListenerCallback(ChangeBroadcaster*) override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    ParametricEQProcessor& processor;

    GroupComponent mainGroup;

    BasicKnob centreFreqKnob; LabeledKnob labeledCentreFreqKnob;
    BasicKnob filterQKnob; LabeledKnob labeledFilterQKnob;
    DecibelKnob gainKnob; LabeledKnob labeledGainKnob;

    Label bandwidthLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParametricEQEditor)
};