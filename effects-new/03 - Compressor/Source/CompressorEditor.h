#pragma once
#include "JuceHeader.h"
#include "CompressorProcessor.h"
#include "BarGraph.h"
#include "LabeledKnob.h"
#include "DecibelKnob.h"

class CompressorEditor  : public AudioProcessorEditor, public ChangeListener
{
public:
    CompressorEditor (CompressorProcessor&);
    virtual ~CompressorEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

    // ChangeListener
    void changeListenerCallback(ChangeBroadcaster*) override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    CompressorProcessor& processor;

    GroupComponent compressionGroup;

    BarGraph barGraph;
    BasicKnob ratioKnob; LabeledKnob labeledRatioKnob;
    BasicKnob attackKnob; LabeledKnob labeledAttackKnob;
    BasicKnob releaseKnob; LabeledKnob labeledReleaseKnob;
    BasicKnob thresholdKnob; LabeledKnob labeledThresholdKnob;
    BasicKnob gainKnob; LabeledKnob labeledGainKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorEditor)
};
