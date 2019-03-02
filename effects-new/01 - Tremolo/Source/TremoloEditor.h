#pragma once
#include "JuceHeader.h"
#include "TremoloProcessor.h"
#include "LabeledKnob.h"

class TremoloEditor  : public AudioProcessorEditor
{
public:
    TremoloEditor (TremoloProcessor&);
    virtual ~TremoloEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    TremoloProcessor& processor;

    GroupComponent tremoloGroup;
    ComboBox lfoWaveformCombo; Label lfoWaveformLabel;
    BasicKnob lfoFreqKnob; LabeledKnob labeledLfoFreqKnob;
    BasicKnob modDepthKnob; LabeledKnob labeledModDepthKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TremoloEditor)
};
