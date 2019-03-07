#include "ChorusProcessor.h"
#include "ChorusEditor.h"

ChorusEditor::ChorusEditor(ChorusProcessor& p)
    : AudioProcessorEditor(&p)
    , processor(p)
    , minDelayKnob(ChorusParameters::minDelayMin, ChorusParameters::minDelayMax, ChorusParameters::minDelayLabel)
    , labeledMinDelayKnob(ChorusParameters::minDelayName, minDelayKnob)
    , sweepWidthKnob(ChorusParameters::sweepWidthMin, ChorusParameters::sweepWidthMax, ChorusParameters::sweepWidthLabel)
    , labeledSweepWidthKnob(ChorusParameters::sweepWidthName, sweepWidthKnob)
    , depthKnob(ChorusParameters::depthMin, ChorusParameters::depthMax, ChorusParameters::depthLabel)
    , labeledDepthKnob(ChorusParameters::depthName, depthKnob)
    , voiceCountKnob(ChorusParameters::voiceCountMin, ChorusParameters::voiceCountMax, ChorusParameters::voiceCountLabel)
    , labeledVoiceCountKnob(ChorusParameters::voiceCountName, voiceCountKnob)
    , lfoFreqKnob(ChorusParameters::lfoFreqMin, ChorusParameters::lfoFreqMax, ChorusParameters::lfoFreqLabel)
    , labeledLfoFreqKnob(ChorusParameters::lfoFreqName, lfoFreqKnob)
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("Chorus");
    addAndMakeVisible(&mainGroup);

    minDelayKnob.setDoubleClickReturnValue(true, double(ChorusParameters::minDelayDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledMinDelayKnob);
    sweepWidthKnob.setDoubleClickReturnValue(true, double(ChorusParameters::sweepWidthDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledSweepWidthKnob);
    depthKnob.setDoubleClickReturnValue(true, double(ChorusParameters::depthDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDepthKnob);
    voiceCountKnob.setDoubleClickReturnValue(true, double(ChorusParameters::voiceCountDefault), ModifierKeys::noModifiers);
    voiceCountKnob.onValueChange = [this]()
    {
        // setting voice count to 2 turns stereo mode off
        if (voiceCountKnob.getValue() < 3.0)
        {
            stereoButton.setToggleState(false, sendNotification);
        }
    };
    addAndMakeVisible(labeledVoiceCountKnob);
    lfoFreqKnob.setDoubleClickReturnValue(true, double(ChorusParameters::lfoFreqDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLfoFreqKnob);
    stereoButton.setButtonText("Stereo");
    stereoButton.onStateChange = [this]()
    {
        // turning on stereo requires at least three voices
        bool stereo = stereoButton.getToggleState();
        int voiceCount = int(voiceCountKnob.getValue());
        if (stereo && (voiceCount < 3))
        {
            voiceCountKnob.setValue(3.0, sendNotification);
        }
    };
    addAndMakeVisible(&stereoButton);
    lfoWaveformLabel.setText("LFO", dontSendNotification);
    lfoWaveformLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&lfoWaveformLabel);
    lfoWaveformCombo.setEditableText(false);
    lfoWaveformCombo.setJustificationType(Justification::centredLeft);
    ChorusLFO::populateWaveformComboBox(lfoWaveformCombo);
    addAndMakeVisible(lfoWaveformCombo);
    interpTypeLabel.setText("Interpolation Type", dontSendNotification);
    interpTypeLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&interpTypeLabel);
    interpTypeCombo.setEditableText(false);
    interpTypeCombo.setJustificationType(Justification::centredLeft);
    ChorusInterpolation::populateInterpolationComboBox(interpTypeCombo);
    addAndMakeVisible(interpTypeCombo);

    processor.parameters.attachControls(
        minDelayKnob,
        sweepWidthKnob,
        depthKnob,
        voiceCountKnob,
        lfoFreqKnob,
        stereoButton,
        lfoWaveformCombo,
        interpTypeCombo );

    setSize (60 + 100 * 8 + 10 * (8 - 1), 180);
}

ChorusEditor::~ChorusEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void ChorusEditor::resized()
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
    labeledVoiceCountKnob.setBounds(widgetsArea.removeFromLeft(width));
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

void ChorusEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}