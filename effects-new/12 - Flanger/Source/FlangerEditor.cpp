#include "FlangerProcessor.h"
#include "FlangerEditor.h"

FlangerEditor::FlangerEditor (FlangerProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , minDelayKnob(FlangerParameters::minDelayMin, FlangerParameters::minDelayMax, FlangerParameters::minDelayLabel)
    , labeledMinDelayKnob(FlangerParameters::minDelayName, minDelayKnob)
    , sweepWidthKnob(FlangerParameters::sweepWidthMin, FlangerParameters::sweepWidthMax, FlangerParameters::sweepWidthLabel)
    , labeledSweepWidthKnob(FlangerParameters::sweepWidthName, sweepWidthKnob)
    , depthKnob(FlangerParameters::depthMin, FlangerParameters::depthMax, FlangerParameters::depthLabel)
    , labeledDepthKnob(FlangerParameters::depthName, depthKnob)
    , feedbackKnob(FlangerParameters::feedbackMin, FlangerParameters::feedbackMax, FlangerParameters::feedbackLabel)
    , labeledFeedbackKnob(FlangerParameters::feedbackName, feedbackKnob)
    , lfoFreqKnob(FlangerParameters::lfoFreqMin, FlangerParameters::lfoFreqMax, FlangerParameters::lfoFreqLabel)
    , labeledLfoFreqKnob(FlangerParameters::lfoFreqName, lfoFreqKnob)
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("Flanger");
    addAndMakeVisible(&mainGroup);

    minDelayKnob.setDoubleClickReturnValue(true, double(FlangerParameters::minDelayDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledMinDelayKnob);
    sweepWidthKnob.setDoubleClickReturnValue(true, double(FlangerParameters::sweepWidthDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledSweepWidthKnob);
    depthKnob.setDoubleClickReturnValue(true, double(FlangerParameters::depthDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDepthKnob);
    feedbackKnob.setDoubleClickReturnValue(true, double(FlangerParameters::feedbackDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledFeedbackKnob);
    lfoFreqKnob.setDoubleClickReturnValue(true, double(FlangerParameters::lfoFreqDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLfoFreqKnob);
    stereoButton.setButtonText("Stereo");
    addAndMakeVisible(&stereoButton);
    lfoWaveformLabel.setText("LFO", dontSendNotification);
    lfoWaveformLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&lfoWaveformLabel);
    lfoWaveformCombo.setEditableText(false);
    lfoWaveformCombo.setJustificationType(Justification::centredLeft);
    FlangerLFO::populateWaveformComboBox(lfoWaveformCombo);
    addAndMakeVisible(lfoWaveformCombo);
    interpTypeLabel.setText("Interpolation Type", dontSendNotification);
    interpTypeLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&interpTypeLabel);
    interpTypeCombo.setEditableText(false);
    interpTypeCombo.setJustificationType(Justification::centredLeft);
    FlangerInterpolation::populateInterpolationComboBox(interpTypeCombo);
    addAndMakeVisible(interpTypeCombo);

    processor.parameters.attachControls(
        minDelayKnob,
        sweepWidthKnob,
        depthKnob,
        feedbackKnob,
        lfoFreqKnob,
        stereoButton,
        lfoWaveformCombo,
        interpTypeCombo );

    setSize (60 + 100 * 8 + 10 * (8 - 1), 180);
}

FlangerEditor::~FlangerEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void FlangerEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    mainGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);
    widgetsArea.removeFromTop(20);
    int width = (widgetsArea.getWidth() - (8 - 1) * 10) / 8;
    labeledMinDelayKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledSweepWidthKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledDepthKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledFeedbackKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledLfoFreqKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(15);

    widgetsArea.removeFromRight(15);
    auto rowArea = widgetsArea.removeFromTop(20);
    rowArea.removeFromLeft(130);
    stereoButton.setBounds(rowArea);
    widgetsArea.removeFromTop(10);
    rowArea = widgetsArea.removeFromTop(24);
    lfoWaveformLabel.setBounds(rowArea.removeFromLeft(120));
    rowArea.removeFromLeft(10);
    lfoWaveformCombo.setBounds(rowArea);
    widgetsArea.removeFromTop(10);
    rowArea = widgetsArea.removeFromTop(24);
    interpTypeLabel.setBounds(rowArea.removeFromLeft(120));
    rowArea.removeFromLeft(10);
    interpTypeCombo.setBounds(rowArea);
}

void FlangerEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}