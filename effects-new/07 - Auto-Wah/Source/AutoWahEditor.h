#pragma once
#include "JuceHeader.h"
#include "AutoWahProcessor.h"
#include "LabeledKnob.h"

class AutoWahEditor  : public AudioProcessorEditor
{
public:
    AutoWahEditor (AutoWahProcessor&);
    virtual ~AutoWahEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    AutoWahProcessor& processor;

    GroupComponent filterGroup, lfoGroup, envelopeGroup;

    BasicKnob centreFreqKnob; LabeledKnob labeledCentreFreqKnob;
    BasicKnob filterQKnob; LabeledKnob labeledFilterQKnob;
    BasicKnob lfoFreqKnob; LabeledKnob labeledLfoFreqKnob;
    BasicKnob lfoWidthKnob; LabeledKnob labeledLfoWidthKnob;
    BasicKnob attackKnob; LabeledKnob labeledAttackKnob;
    BasicKnob decayKnob; LabeledKnob labeledReleaseKnob;
    BasicKnob envWidthKnob; LabeledKnob labeledEnvWidthKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoWahEditor)
};