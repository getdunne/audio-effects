#include "PhaserProcessor.h"
#include "PhaserEditor.h"

PhaserEditor::PhaserEditor (PhaserProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , baseFreqKnob(PhaserParameters::baseFreqMin, PhaserParameters::baseFreqMax, "Hz")
    , labeledBaseFreqKnob("Base Freq", baseFreqKnob)
    , sweepWidthKnob(PhaserParameters::sweepWidthMin, PhaserParameters::sweepWidthMax, "Hz")
    , labeledSweepWidthKnob("Sweep Width", sweepWidthKnob)
    , depthKnob(PhaserParameters::depthMin, PhaserParameters::depthMax, "%")
    , labeledDepthKnob("Depth", depthKnob)
    , feedbackKnob(PhaserParameters::feedbackMin, PhaserParameters::feedbackMax, "%")
    , labeledFeedbackKnob("Feedback", feedbackKnob)
    , lfoFreqKnob(PhaserParameters::lfoFreqMin, PhaserParameters::lfoFreqMax, "Hz")
    , labeledLfoFreqKnob("LFO Freq", lfoFreqKnob)
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("Phaser");
    addAndMakeVisible(&mainGroup);

    baseFreqKnob.setDoubleClickReturnValue(true, double(PhaserParameters::baseFreqDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledBaseFreqKnob);
    sweepWidthKnob.setDoubleClickReturnValue(true, double(PhaserParameters::sweepWidthDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledSweepWidthKnob);
    depthKnob.setDoubleClickReturnValue(true, double(PhaserParameters::depthDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDepthKnob);
    feedbackKnob.setDoubleClickReturnValue(true, double(PhaserParameters::feedbackDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledFeedbackKnob);
    lfoFreqKnob.setDoubleClickReturnValue(true, double(PhaserParameters::lfoFreqDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLfoFreqKnob);
    stereoButton.setButtonText("Stereo");
    addAndMakeVisible(stereoButton);
    lfoWaveformLabel.setText("LFO Waveform", dontSendNotification);
    lfoWaveformLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&lfoWaveformLabel);
    lfoWaveformCombo.setEditableText(false);
    lfoWaveformCombo.setJustificationType(Justification::centredLeft);
    PhaserLFO::populateWaveformComboBox(lfoWaveformCombo);
    addAndMakeVisible(lfoWaveformCombo);
    numFiltersLabel.setText("Number of Filters", dontSendNotification);
    numFiltersLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&numFiltersLabel);
    numFiltersCombo.setEditableText(false);
    numFiltersCombo.setJustificationType(Justification::centredLeft);
    for (int i = 2; i <= 10; i += 2)
        numFiltersCombo.addItem(String(i), i);
    addAndMakeVisible(numFiltersCombo);

    processor.parameters.attachControls(
        baseFreqKnob,
        sweepWidthKnob,
        depthKnob,
        feedbackKnob,
        lfoFreqKnob,
        stereoButton,
        lfoWaveformCombo,
        numFiltersCombo);

    setSize (60 + 100 * 8 + 10 * (8 - 1), 180);
}

PhaserEditor::~PhaserEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void PhaserEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    mainGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);
    widgetsArea.removeFromTop(20);
    int width = (widgetsArea.getWidth() - (8 - 1) * 10) / 8;
    labeledBaseFreqKnob.setBounds(widgetsArea.removeFromLeft(width));
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
    numFiltersLabel.setBounds(rowArea.removeFromLeft(120));
    rowArea.removeFromLeft(10);
    numFiltersCombo.setBounds(rowArea);
}

void PhaserEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}