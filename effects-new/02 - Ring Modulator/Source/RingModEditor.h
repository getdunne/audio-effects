#pragma once
#include "JuceHeader.h"
#include "RingModProcessor.h"
#include "LabeledKnob.h"

class RingModEditor  : public AudioProcessorEditor
{
public:
    RingModEditor (RingModProcessor&);
    virtual ~RingModEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    RingModProcessor& processor;

    GroupComponent carrierGroup;
    BasicKnob carrierFreqKnob; LabeledKnob labeledCarrierFreqKnob;

    GroupComponent lfoGroup;
    ComboBox lfoWaveformCombo; Label lfoWaveformLabel;
    BasicKnob lfoFreqKnob; LabeledKnob labeledLfoFreqKnob;
    BasicKnob lfoWidthKnob; LabeledKnob labeledLfoWidthKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RingModEditor)
};
