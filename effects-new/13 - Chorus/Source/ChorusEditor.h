#pragma once
#include "JuceHeader.h"
#include "ChorusProcessor.h"
#include "LabeledKnob.h"

class ChorusEditor  : public AudioProcessorEditor
{
public:
    ChorusEditor (ChorusProcessor&);
    virtual ~ChorusEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    ChorusProcessor& processor;

    GroupComponent mainGroup;

    BasicKnob minDelayKnob; LabeledKnob labeledMinDelayKnob;
    BasicKnob sweepWidthKnob; LabeledKnob labeledSweepWidthKnob;
    BasicKnob depthKnob; LabeledKnob labeledDepthKnob;
    BasicKnob voiceCountKnob; LabeledKnob labeledVoiceCountKnob;
    BasicKnob lfoFreqKnob; LabeledKnob labeledLfoFreqKnob;
    ToggleButton stereoButton;
    ComboBox lfoWaveformCombo; Label lfoWaveformLabel;
    ComboBox interpTypeCombo; Label interpTypeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusEditor)
};