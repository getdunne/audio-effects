#include "AutoWahProcessor.h"
#include "AutoWahEditor.h"

AutoWahEditor::AutoWahEditor (AutoWahProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , centreFreqKnob(200.0f, 1000.0f, "Hz"), labeledCentreFreqKnob("Centre Freq", centreFreqKnob)
    , filterQKnob(2.0f, 20.0f, ""), labeledFilterQKnob("Q", filterQKnob)
    , lfoFreqKnob(0.2f, 20.0f, "Hz"), labeledLfoFreqKnob("LFO Freq", lfoFreqKnob)
    , lfoDepthKnob(0.0f, 2000.0f, "Hz"), labeledLfoDepthKnob("LFO Depth", lfoDepthKnob)
    , attackKnob(0.0f, 0.1f, "sec"), labeledAttackKnob("Attack Time", attackKnob)
    , releaseKnob(0.0f, 2.0f, "sec"), labeledReleaseKnob("Release Time", releaseKnob)
    , modDepthKnob(0.0f, 100.0f, "%"), labeledModDepthKnob("Env Depth", modDepthKnob)
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("AutoWah");
    addAndMakeVisible(&mainGroup);

    centreFreqKnob.setDoubleClickReturnValue(true, 350.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledCentreFreqKnob);
    filterQKnob.setDoubleClickReturnValue(true, 5.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledFilterQKnob);
    lfoFreqKnob.setDoubleClickReturnValue(true, 2.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLfoFreqKnob);
    lfoDepthKnob.setDoubleClickReturnValue(true, 1000.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLfoDepthKnob);
    attackKnob.setDoubleClickReturnValue(true, 0.005, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledAttackKnob);
    releaseKnob.setDoubleClickReturnValue(true, 0.1, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledReleaseKnob);
    modDepthKnob.setDoubleClickReturnValue(true, 0.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledModDepthKnob);

    processor.parameters.attachControls(
        centreFreqKnob,
        filterQKnob,
        lfoFreqKnob,
        lfoDepthKnob,
        attackKnob,
        releaseKnob,
        modDepthKnob );

    setSize (60 + 100 * 7 + 10 * (7 - 1), 180);
}

AutoWahEditor::~AutoWahEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void AutoWahEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    mainGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);
    widgetsArea.removeFromTop(20);
    int width = (widgetsArea.getWidth() - (7 - 1) * 10) / 7;
    labeledCentreFreqKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledFilterQKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledLfoFreqKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledLfoDepthKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledAttackKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledReleaseKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledModDepthKnob.setBounds(widgetsArea);
}

void AutoWahEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}