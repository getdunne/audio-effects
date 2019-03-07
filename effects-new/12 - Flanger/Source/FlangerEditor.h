#pragma once
#include "JuceHeader.h"
#include "FlangerProcessor.h"
#include "LabeledKnob.h"

class FlangerEditor  : public AudioProcessorEditor
{
public:
    FlangerEditor (FlangerProcessor&);
    virtual ~FlangerEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    FlangerProcessor& processor;

    GroupComponent mainGroup;

    BasicKnob minDelayKnob; LabeledKnob labeledMinDelayKnob;
    BasicKnob sweepWidthKnob; LabeledKnob labeledSweepWidthKnob;
    BasicKnob depthKnob; LabeledKnob labeledDepthKnob;
    BasicKnob feedbackKnob; LabeledKnob labeledFeedbackKnob;
    BasicKnob lfoFreqKnob; LabeledKnob labeledLfoFreqKnob;
    ToggleButton stereoButton;
    ComboBox lfoWaveformCombo; Label lfoWaveformLabel;
    ComboBox interpTypeCombo; Label interpTypeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlangerEditor)
};