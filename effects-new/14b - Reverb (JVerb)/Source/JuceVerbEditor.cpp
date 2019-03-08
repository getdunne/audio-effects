#include "JuceVerbProcessor.h"
#include "JuceVerbEditor.h"

JuceVerbEditor::JuceVerbEditor (JuceVerbProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , roomSizeKnob(JuceVerbParameters::roomSizeMin, JuceVerbParameters::roomSizeMax, JuceVerbParameters::roomSizeLabel)
    , labeledRoomSizeKnob(JuceVerbParameters::roomSizeName, roomSizeKnob)
    , dampingKnob(JuceVerbParameters::dampingMin, JuceVerbParameters::dampingMax, JuceVerbParameters::dampingLabel)
    , labeledDampingKnob(JuceVerbParameters::dampingName, dampingKnob)
    , wetLevelKnob(JuceVerbParameters::wetLevelMin, JuceVerbParameters::wetLevelMax, JuceVerbParameters::wetLevelLabel)
    , labeledWetLevelKnob(JuceVerbParameters::wetLevelName, wetLevelKnob)
    , dryLevelKnob(JuceVerbParameters::dryLevelMin, JuceVerbParameters::dryLevelMax, JuceVerbParameters::dryLevelLabel)
    , labeledDryLevelKnob(JuceVerbParameters::dryLevelName, dryLevelKnob)
    , widthKnob(JuceVerbParameters::widthMin, JuceVerbParameters::widthMax, JuceVerbParameters::widthLabel)
    , labeledWidthKnob(JuceVerbParameters::widthName, widthKnob)
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("JVerb");
    addAndMakeVisible(&mainGroup);

    roomSizeKnob.setDoubleClickReturnValue(true, double(JuceVerbParameters::roomSizeDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledRoomSizeKnob);
    dampingKnob.setDoubleClickReturnValue(true, double(JuceVerbParameters::dampingDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDampingKnob);
    wetLevelKnob.setDoubleClickReturnValue(true, double(JuceVerbParameters::wetLevelDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledWetLevelKnob);
    dryLevelKnob.setDoubleClickReturnValue(true, double(JuceVerbParameters::dryLevelDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDryLevelKnob);
    widthKnob.setDoubleClickReturnValue(true, double(JuceVerbParameters::widthDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledWidthKnob);
    freezeModeToggle.setButtonText("Freeze");
    addAndMakeVisible(freezeModeToggle);

    processor.parameters.attachControls(
        roomSizeKnob,
        dampingKnob,
        wetLevelKnob,
        dryLevelKnob,
        widthKnob,
        freezeModeToggle);

    setSize (700, 180);
}

JuceVerbEditor::~JuceVerbEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void JuceVerbEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    mainGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);
    widgetsArea.removeFromTop(20);
    int width = (widgetsArea.getWidth() - (6 - 1) * 10) / 6;
    labeledRoomSizeKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledDampingKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledWetLevelKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledDryLevelKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledWidthKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(15);

    widgetsArea.removeFromRight(15);
    widgetsArea.removeFromTop(30);
    auto rowArea = widgetsArea.removeFromTop(20);
    freezeModeToggle.setBounds(rowArea);
}

void JuceVerbEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}