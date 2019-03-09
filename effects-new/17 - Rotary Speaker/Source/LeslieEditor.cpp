#include "LeslieProcessor.h"
#include "LeslieEditor.h"

LeslieEditor::LeslieEditor (LeslieProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , speedKnob(LeslieParameters::speedMin, LeslieParameters::speedMax, LeslieParameters::speedLabel)
    , labeledSpeedKnob(LeslieParameters::speedName, speedKnob)
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("Rotary Speaker");
    addAndMakeVisible(&mainGroup);

    speedKnob.setDoubleClickReturnValue(true, double(LeslieParameters::speedDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledSpeedKnob);

    processor.parameters.attachControls(speedKnob);

    midiSustainToggle.setButtonText("Sustain Pedal controls speed");
    midiSustainToggle.setToggleState(processor.midiSustainControlsLeslieSpeed, dontSendNotification);
    midiSustainToggle.onClick = [this]()
    {
        processor.midiSustainControlsLeslieSpeed = midiSustainToggle.getToggleState();
        if (processor.midiModWheelControlsLeslieSpeed)
        {
            processor.midiModWheelControlsLeslieSpeed = false;
            midiModWheelToggle.setToggleState(false, dontSendNotification);
        }
    };
    addAndMakeVisible(midiSustainToggle);

    midiModWheelToggle.setButtonText("Mod Wheel controls speed");
    midiModWheelToggle.setToggleState(processor.midiModWheelControlsLeslieSpeed, dontSendNotification);
    midiModWheelToggle.onClick = [this]()
    {
        processor.midiModWheelControlsLeslieSpeed = midiModWheelToggle.getToggleState();
        if (processor.midiSustainControlsLeslieSpeed)
        {
            processor.midiSustainControlsLeslieSpeed = false;
            midiSustainToggle.setToggleState(false, dontSendNotification);
        }
    };
    addAndMakeVisible(midiModWheelToggle);

    setSize (400, 180);
}

LeslieEditor::~LeslieEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void LeslieEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    mainGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);
    widgetsArea.removeFromTop(20);
    labeledSpeedKnob.setBounds(widgetsArea.removeFromLeft(100));
    widgetsArea.removeFromLeft(15);

    widgetsArea.removeFromRight(15);
    widgetsArea.removeFromTop(16);
    midiSustainToggle.setBounds(widgetsArea.removeFromTop(20));
    widgetsArea.removeFromTop(10);
    midiModWheelToggle.setBounds(widgetsArea.removeFromTop(20));
}

void LeslieEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}