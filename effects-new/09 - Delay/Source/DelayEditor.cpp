#include "DelayProcessor.h"
#include "DelayEditor.h"

DelayEditor::DelayEditor (DelayProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , delayTimeKnob(DelayParameters::delayTimeMin, DelayParameters::delayTimeMax, DelayParameters::delayTimeLabel)
    , labeledDelayTimeKnob(DelayParameters::delayTimeName, delayTimeKnob)
    , feedbackKnob(DelayParameters::feedbackMin, DelayParameters::feedbackMax, DelayParameters::feedbackLabel)
    , labeledFeedbackKnob(DelayParameters::feedbackName, feedbackKnob)
    , dryLevelKnob(DelayParameters::dryLevelMin, DelayParameters::dryLevelMax, DelayParameters::dryLevelLabel)
    , labeledDryLevelKnob(DelayParameters::dryLevelName, dryLevelKnob)
    , wetLevelKnob(DelayParameters::wetLevelMin, DelayParameters::wetLevelMax, DelayParameters::wetLevelLabel)
    , labeledWetLevelKnob(DelayParameters::wetLevelName, wetLevelKnob)
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("Delay");
    addAndMakeVisible(&mainGroup);

    delayTimeKnob.setDoubleClickReturnValue(true, double(DelayParameters::delayTimeDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDelayTimeKnob);
    feedbackKnob.setDoubleClickReturnValue(true, double(DelayParameters::feedbackDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledFeedbackKnob);
    dryLevelKnob.setDoubleClickReturnValue(true, double(DelayParameters::dryLevelDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDryLevelKnob);
    wetLevelKnob.setDoubleClickReturnValue(true, double(DelayParameters::wetLevelDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledWetLevelKnob);

    processor.parameters.attachControls(
        delayTimeKnob,
        feedbackKnob,
        dryLevelKnob,
        wetLevelKnob );

    setSize (60 + 100 * 4 + 10 * (4 - 1), 180);
}

DelayEditor::~DelayEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void DelayEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    mainGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);
    widgetsArea.removeFromTop(20);
    int width = (widgetsArea.getWidth() - (4 - 1) * 10) / 4;
    labeledDelayTimeKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledFeedbackKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledDryLevelKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledWetLevelKnob.setBounds(widgetsArea);
}

void DelayEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}