#include "PingPongDelayProcessor.h"
#include "PingPongDelayEditor.h"

PingPongDelayEditor::PingPongDelayEditor (PingPongDelayProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , delayTimeLKnob(PingPongDelayParameters::delayTimeLMin, PingPongDelayParameters::delayTimeLMax, PingPongDelayParameters::delayTimeLLabel)
    , labeledDelayTimeLKnob(PingPongDelayParameters::delayTimeLName, delayTimeLKnob)
    , delayTimeRKnob(PingPongDelayParameters::delayTimeRMin, PingPongDelayParameters::delayTimeRMax, PingPongDelayParameters::delayTimeRLabel)
    , labeledDelayTimeRKnob(PingPongDelayParameters::delayTimeRName, delayTimeRKnob)
    , feedbackKnob(PingPongDelayParameters::feedbackMin, PingPongDelayParameters::feedbackMax, PingPongDelayParameters::feedbackLabel)
    , labeledFeedbackKnob(PingPongDelayParameters::feedbackName, feedbackKnob)
    , wetLevelKnob(PingPongDelayParameters::wetLevelMin, PingPongDelayParameters::wetLevelMax, PingPongDelayParameters::wetLevelLabel)
    , labeledWetLevelKnob(PingPongDelayParameters::wetLevelName, wetLevelKnob)
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("PingPongDelay");
    addAndMakeVisible(&mainGroup);

    delayTimeLKnob.setDoubleClickReturnValue(true, double(PingPongDelayParameters::delayTimeLDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDelayTimeLKnob);
    delayTimeRKnob.setDoubleClickReturnValue(true, double(PingPongDelayParameters::delayTimeRDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDelayTimeRKnob);
    feedbackKnob.setDoubleClickReturnValue(true, double(PingPongDelayParameters::feedbackDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledFeedbackKnob);
    wetLevelKnob.setDoubleClickReturnValue(true, double(PingPongDelayParameters::wetLevelDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledWetLevelKnob);

    processor.parameters.attachControls(
        delayTimeLKnob,
        delayTimeRKnob,
        feedbackKnob,
        wetLevelKnob );

    setSize (60 + 100 * 4 + 10 * (4 - 1), 180);
}

PingPongDelayEditor::~PingPongDelayEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void PingPongDelayEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    mainGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);
    widgetsArea.removeFromTop(20);
    int width = (widgetsArea.getWidth() - (4 - 1) * 10) / 4;
    labeledDelayTimeLKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledDelayTimeRKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledFeedbackKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledWetLevelKnob.setBounds(widgetsArea);
}

void PingPongDelayEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}