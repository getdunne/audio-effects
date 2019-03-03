#include "CompressorProcessor.h"
#include "CompressorEditor.h"

CompressorEditor::CompressorEditor (CompressorProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , barGraph(Colours::steelblue)
    , ratioKnob(0.0f, 100.0f), labeledRatioKnob("Ratio", ratioKnob)
    , attackKnob(0.1f, 80.0f, "ms"), labeledAttackKnob("Attack", attackKnob)
    , releaseKnob(0.1f, 1000.0f, "ms"), labeledReleaseKnob("Release", releaseKnob)
    , thresholdKnob(-60.0f, 0.0f, "dB"), labeledThresholdKnob("Threshold", thresholdKnob)
    , gainKnob(0.0f, 40.0f, "dB"), labeledGainKnob("Gain", gainKnob)
{
    setLookAndFeel(lookAndFeel);

    compressionGroup.setText("Compressor");
    addAndMakeVisible(&compressionGroup);

    barGraph.setValue(1.0f);
    addAndMakeVisible(&barGraph);

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

    processor.addChangeListener(this);

    setSize (600, 200);
}

CompressorEditor::~CompressorEditor()
{
    processor.removeChangeListener(this);
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void CompressorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    compressionGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);
    widgetsArea.removeFromTop(20);
    barGraph.setBounds(widgetsArea.removeFromTop(8).reduced(20, 0));
    widgetsArea.removeFromTop(12);
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

void CompressorEditor::changeListenerCallback(ChangeBroadcaster*)
{
    barGraph.setValue(processor.currentGain);
}
