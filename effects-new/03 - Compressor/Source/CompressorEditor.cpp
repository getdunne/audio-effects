#include "CompressorProcessor.h"
#include "CompressorEditor.h"

CompressorEditor::CompressorEditor (CompressorProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , ratioKnob(0.0f, 100.0f, 0.1f), labeledRatioKnob("Ratio", ratioKnob)
    , attackKnob(0.1f, 80.0f, 0.1f, "ms"), labeledAttackKnob("Attack", attackKnob)
    , releaseKnob(0.1f, 1000.0f, 0.1f, "ms"), labeledReleaseKnob("Release", releaseKnob)
    , thresholdKnob(-60.0f, 0.0f, 0.5f, "dB"), labeledThresholdKnob("Threshold", thresholdKnob)
    , gainKnob(0.0f, 40.0f, 0.1f, "dB"), labeledGainKnob("Gain", gainKnob)
{
    setLookAndFeel(lookAndFeel);

    compressionGroup.setText("Compressor");
    addAndMakeVisible(&compressionGroup);

    ratioKnob.setDoubleClickReturnValue(true, 0.1, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledRatioKnob);

    attackKnob.setDoubleClickReturnValue(true, 0.1, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledAttackKnob);

    releaseKnob.setDoubleClickReturnValue(true, 0.1, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledReleaseKnob);

    thresholdKnob.setDoubleClickReturnValue(true, 0.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledThresholdKnob);

    gainKnob.setDoubleClickReturnValue(true, 0.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledGainKnob);

    processor.parameters.attachControls(ratioKnob, attackKnob, releaseKnob, thresholdKnob, gainKnob);

    setSize (600, 180);
}

CompressorEditor::~CompressorEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void CompressorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    compressionGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);
    widgetsArea.removeFromTop(10);
    int width = (widgetsArea.getWidth() - 4 * 10) / 5;
    labeledRatioKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledAttackKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledReleaseKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledThresholdKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledGainKnob.setBounds(widgetsArea);
}

void CompressorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}
