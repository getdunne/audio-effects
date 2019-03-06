#include "VibratoProcessor.h"
#include "VibratoEditor.h"

VibratoEditor::VibratoEditor (VibratoProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , lfoFreqKnob(VibratoParameters::lfoFreqMin, VibratoParameters::lfoFreqMax, VibratoParameters::lfoFreqLabel)
    , labeledLfoFreqKnob(VibratoParameters::lfoFreqName, lfoFreqKnob)
    , sweepWidthKnob(VibratoParameters::sweepWidthMin, VibratoParameters::sweepWidthMax, VibratoParameters::sweepWidthLabel)
    , labeledSweepWidthKnob(VibratoParameters::sweepWidthName, sweepWidthKnob)
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("Vibrato");
    addAndMakeVisible(&mainGroup);

    lfoFreqKnob.setDoubleClickReturnValue(true, double(VibratoParameters::lfoFreqDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLfoFreqKnob);
    sweepWidthKnob.setDoubleClickReturnValue(true, double(VibratoParameters::sweepWidthDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledSweepWidthKnob);

    lfoWaveformLabel.setText("LFO", dontSendNotification);
    lfoWaveformLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&lfoWaveformLabel);
    lfoWaveformCombo.setEditableText(false);
    lfoWaveformCombo.setJustificationType(Justification::centredLeft);
    VibratoLFO::populateWaveformComboBox(lfoWaveformCombo);
    addAndMakeVisible(lfoWaveformCombo);

    interpTypeLabel.setText("Interpolation Type", dontSendNotification);
    interpTypeLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&interpTypeLabel);
    interpTypeCombo.setEditableText(false);
    interpTypeCombo.setJustificationType(Justification::centredLeft);
    VibratoInterpolation::populateInterpolationComboBox(interpTypeCombo);
    addAndMakeVisible(interpTypeCombo);

    addAndMakeVisible(pitchShiftDescriptionLabel);

    processor.addChangeListener(this);
    processor.parameters.attachControls(
        lfoWaveformCombo,
        lfoFreqKnob,
        sweepWidthKnob,
        interpTypeCombo );

    setSize (600, 220);
}

VibratoEditor::~VibratoEditor()
{
    processor.removeChangeListener(this);
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void VibratoEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    mainGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);

    widgetsArea.removeFromTop(20);
    pitchShiftDescriptionLabel.setBounds(widgetsArea.removeFromTop(20).reduced(40, 0));

    widgetsArea.removeFromTop(20);
    int knobWidth = 100;
    labeledLfoFreqKnob.setBounds(widgetsArea.removeFromLeft(knobWidth));
    widgetsArea.removeFromLeft(10);
    labeledSweepWidthKnob.setBounds(widgetsArea.removeFromLeft(knobWidth));
    widgetsArea.removeFromLeft(15);

    widgetsArea.removeFromRight(15);
    widgetsArea.removeFromTop(16);
    auto rowArea = widgetsArea.removeFromTop(24);
    lfoWaveformLabel.setBounds(rowArea.removeFromLeft(120));
    rowArea.removeFromLeft(10);
    lfoWaveformCombo.setBounds(rowArea);
    widgetsArea.removeFromTop(10);
    rowArea = widgetsArea.removeFromTop(24);
    interpTypeLabel.setBounds(rowArea.removeFromLeft(120));
    rowArea.removeFromLeft(10);
    interpTypeCombo.setBounds(rowArea);
}

void VibratoEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}

void VibratoEditor::changeListenerCallback(ChangeBroadcaster*)
{
    pitchShiftDescriptionLabel.setText(processor.pitchShiftDescription, dontSendNotification);
}
