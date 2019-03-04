#include "DistortionProcessor.h"
#include "DistortionEditor.h"
#include "Distortion.h"

DistortionEditor::DistortionEditor (DistortionProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , gainKnob(-24.0f, 24.0f), labeledGainKnob("Gain", gainKnob)
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("Distortion");
    addAndMakeVisible(&mainGroup);

    distTypeLabel.setText("Type", dontSendNotification);
    distTypeLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&distTypeLabel);

    distTypeCombo.setEditableText(false);
    distTypeCombo.setJustificationType(Justification::centredLeft);
    Distortion::populateDistortionTypeComboBox(distTypeCombo);
    addAndMakeVisible(distTypeCombo);

    gainKnob.setDoubleClickReturnValue(true, 0.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledGainKnob);

    processor.parameters.attachControls(
        distTypeCombo,
        gainKnob);

    setSize (60 + 100 * 2 + 10 * (2 - 1), 220);
}

DistortionEditor::~DistortionEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void DistortionEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    mainGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);

    widgetsArea.removeFromTop(12);
    auto waveformArea = widgetsArea.removeFromTop(24);
    distTypeLabel.setBounds(waveformArea.removeFromLeft(40));
    distTypeCombo.setBounds(waveformArea);
    widgetsArea.removeFromTop(10);

    labeledGainKnob.setBounds(widgetsArea);
}

void DistortionEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}
