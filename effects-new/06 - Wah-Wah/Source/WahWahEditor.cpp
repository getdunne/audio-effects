#include "WahWahProcessor.h"
#include "WahWahEditor.h"

WahWahEditor::WahWahEditor (WahWahProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , centreFreqKnob(400.0f, 2000.0f, "Hz"), labeledCentreFreqKnob("Centre Freq", centreFreqKnob)
    , filterQKnob(2.0f, 20.0f), labeledFilterQKnob("Q", filterQKnob)
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("WahWah");
    addAndMakeVisible(&mainGroup);

    centreFreqKnob.setDoubleClickReturnValue(true, 400.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledCentreFreqKnob);
    filterQKnob.setDoubleClickReturnValue(true, 5.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledFilterQKnob);

    processor.parameters.attachControls(
        centreFreqKnob,
        filterQKnob );

    setSize (60 + 100 * 2 + 10 * (2 - 1), 180);
}

WahWahEditor::~WahWahEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void WahWahEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    mainGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);
    widgetsArea.removeFromTop(20);
    int width = (widgetsArea.getWidth() - (2 - 1) * 10) / 2;
    labeledCentreFreqKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledFilterQKnob.setBounds(widgetsArea);
}

void WahWahEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}