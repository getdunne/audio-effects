#include "ParametricEQProcessor.h"
#include "ParametricEQEditor.h"

ParametricEQEditor::ParametricEQEditor (ParametricEQProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , centreFreqKnob(10.0f, 20000.0f), labeledCentreFreqKnob("Centre Freq", centreFreqKnob)
    , filterQKnob(0.1f, 20.0f), labeledFilterQKnob("Q", filterQKnob)
    , gainKnob(-12.0f, 12.0f), labeledGainKnob("Gain", gainKnob)
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("ParametricEQ");
    addAndMakeVisible(&mainGroup);

    centreFreqKnob.setDoubleClickReturnValue(true, 1000.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledCentreFreqKnob);
    filterQKnob.setDoubleClickReturnValue(true, 2.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledFilterQKnob);
    gainKnob.setDoubleClickReturnValue(true, 0.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledGainKnob);

    addAndMakeVisible(bandwidthLabel);

    processor.parameters.attachControls(
        centreFreqKnob,
        filterQKnob,
        gainKnob );

    processor.addChangeListener(this);
    changeListenerCallback(nullptr);    // force 1st update of bandwidthLabel

    setSize (60 + 100 * 3 + 10 * (3 - 1), 200);
}

ParametricEQEditor::~ParametricEQEditor()
{
    processor.removeChangeListener(this);
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void ParametricEQEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    mainGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);
    widgetsArea.removeFromTop(16);

    bandwidthLabel.setBounds(widgetsArea.removeFromTop(20).reduced(20, 0));
    widgetsArea.removeFromTop(4);

    int width = (widgetsArea.getWidth() - (3 - 1) * 10) / 3;
    labeledCentreFreqKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledFilterQKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledGainKnob.setBounds(widgetsArea);
}

void ParametricEQEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}

void ParametricEQEditor::changeListenerCallback(ChangeBroadcaster*)
{
    String bws = "Bandwidth " + String(processor.parameters.getBandwidthHz()) + " Hz";
    bandwidthLabel.setText(bws, dontSendNotification);
}
