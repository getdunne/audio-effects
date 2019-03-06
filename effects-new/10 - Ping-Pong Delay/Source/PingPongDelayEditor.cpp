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

    linkDelaysToggle.setButtonText("Link Delays");
    linkDelaysToggle.setToggleState(processor.parameters.linkDelays, dontSendNotification);
    linkDelaysToggle.onStateChange = [this]()
    {
        processor.parameters.linkDelays = linkDelaysToggle.getToggleState();
        if (processor.parameters.linkDelays)
        {
            auto time = delayTimeLKnob.getValue();
            delayTimeRKnob.setValue(time, sendNotification);
        }
    };
    addAndMakeVisible(linkDelaysToggle);

    delayTimeLKnob.onValueChange = [this]()
    {
        if (processor.parameters.linkDelays)
        {
            auto time = delayTimeLKnob.getValue();
            delayTimeRKnob.setValue(time, sendNotification);
        }
    };
    delayTimeRKnob.onValueChange = [this]()
    {
        if (processor.parameters.linkDelays)
        {
            auto time = delayTimeRKnob.getValue();
            delayTimeLKnob.setValue(time, sendNotification);
        }
    };

    reverseChannelsToggle.setButtonText("Reverse Channels");
    reverseChannelsToggle.setToggleState(processor.parameters.reverseChannels, dontSendNotification);
    reverseChannelsToggle.onStateChange = [this]()
    {
        processor.parameters.reverseChannels = reverseChannelsToggle.getToggleState();
    };
    addAndMakeVisible(reverseChannelsToggle);

    processor.parameters.attachControls(
        delayTimeLKnob,
        delayTimeRKnob,
        feedbackKnob,
        wetLevelKnob );

    setSize (60 + 100 * 4 + 10 * (4 - 1), 220);
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

    auto rowArea = widgetsArea.removeFromTop(24).reduced(40, 0);
    auto width = (rowArea.getWidth() - 20) / 2;
    linkDelaysToggle.setBounds(rowArea.removeFromLeft(width));
    reverseChannelsToggle.setBounds(rowArea.removeFromRight(width));

    widgetsArea.removeFromTop(16);
    width = (widgetsArea.getWidth() - (4 - 1) * 10) / 4;
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
