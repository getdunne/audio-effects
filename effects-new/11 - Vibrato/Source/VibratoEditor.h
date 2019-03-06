#pragma once
#include "JuceHeader.h"
#include "VibratoProcessor.h"
#include "LabeledKnob.h"

class VibratoEditor : public AudioProcessorEditor
                    , public ChangeListener
{
public:
    VibratoEditor (VibratoProcessor&);
    virtual ~VibratoEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

    // ChangeListener
    void changeListenerCallback(ChangeBroadcaster*) override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    VibratoProcessor& processor;

    GroupComponent mainGroup;

    BasicKnob lfoFreqKnob; LabeledKnob labeledLfoFreqKnob;
    BasicKnob sweepWidthKnob; LabeledKnob labeledSweepWidthKnob;
    ComboBox lfoWaveformCombo; Label lfoWaveformLabel;
    ComboBox interpTypeCombo; Label interpTypeLabel;
    Label pitchShiftDescriptionLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VibratoEditor)
};