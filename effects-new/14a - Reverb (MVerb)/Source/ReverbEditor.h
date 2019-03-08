#pragma once
#include "JuceHeader.h"
#include "ReverbProcessor.h"
#include "LabeledKnob.h"

class ReverbEditor  : public AudioProcessorEditor
{
public:
    ReverbEditor (ReverbProcessor&);
    virtual ~ReverbEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    ReverbProcessor& processor;

    GroupComponent mainGroup;

    BasicKnob dampingKnob; LabeledKnob labeledDampingKnob;
    BasicKnob densityKnob; LabeledKnob labeledDensityKnob;
    BasicKnob bandwidthKnob; LabeledKnob labeledBandwidthKnob;
    BasicKnob decayKnob; LabeledKnob labeledDecayKnob;
    BasicKnob predelayKnob; LabeledKnob labeledPredelayKnob;
    BasicKnob roomSizeKnob; LabeledKnob labeledRoomSizeKnob;
    BasicKnob dryWetMixKnob; LabeledKnob labeledDryWetMixKnob;
    BasicKnob earlyLateMixKnob; LabeledKnob labeledEarlyRefMixKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbEditor)
};