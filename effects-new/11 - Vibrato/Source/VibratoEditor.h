#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "VibratoProcessor.h"
#include "BasicKnob.h"
#include "LabeledKnob.h"


class VibratoEditor  : public AudioProcessorEditor
{
public:
    VibratoEditor (VibratoProcessor&);
    virtual ~VibratoEditor();

//    void updatePitchShiftLabel();

    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    VibratoProcessor& processor;

    GroupComponent vibratoGroup;

    ComboBox lfoWaveformCombo, interpolationCombo;
    Label lfoWaveformLabel, interpolationLabel;
    BasicKnob lfoFreqKnob, sweepWidthKnob;
    LabeledKnob labeledLfoFreqKnob, labeledSweepWidthKnob;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VibratoEditor)
};
