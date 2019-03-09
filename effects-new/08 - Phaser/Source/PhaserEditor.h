#pragma once
#include "JuceHeader.h"
#include "PhaserProcessor.h"
#include "LabeledKnob.h"

class PhaserEditor  : public AudioProcessorEditor
{
public:
    PhaserEditor (PhaserProcessor&);
    virtual ~PhaserEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    PhaserProcessor& processor;

    GroupComponent mainGroup;

    BasicKnob baseFreqKnob; LabeledKnob labeledBaseFreqKnob;
    BasicKnob sweepWidthKnob; LabeledKnob labeledSweepWidthKnob;
    BasicKnob depthKnob; LabeledKnob labeledDepthKnob;
    BasicKnob feedbackKnob; LabeledKnob labeledFeedbackKnob;
    BasicKnob lfoFreqKnob; LabeledKnob labeledLfoFreqKnob;
    ToggleButton stereoButton;
    ComboBox lfoWaveformCombo; Label lfoWaveformLabel;
    ComboBox numFiltersCombo; Label numFiltersLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaserEditor)
};
