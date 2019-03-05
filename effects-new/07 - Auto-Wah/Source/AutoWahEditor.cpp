#include "AutoWahProcessor.h"
#include "AutoWahEditor.h"

AutoWahEditor::AutoWahEditor (AutoWahProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , centreFreqKnob(200.0f, 1000.0f, "Hz"), labeledCentreFreqKnob("Centre Freq", centreFreqKnob)
    , filterQKnob(2.0f, 20.0f), labeledFilterQKnob("Q", filterQKnob)
    , lfoFreqKnob(0.2f, 20.0f, "Hz"), labeledLfoFreqKnob("LFO Freq", lfoFreqKnob)
    , lfoWidthKnob(0.0f, 2000.0f, "Hz"), labeledLfoWidthKnob("LFO Width", lfoWidthKnob)
    , attackKnob(0.0f, 2.0f, "sec"), labeledAttackKnob("Attack Time", attackKnob)
    , decayKnob(0.0f, 2.0f, "sec"), labeledReleaseKnob("Decay Time", decayKnob)
    , envWidthKnob(0.0f, 2000.0f, "Hz"), labeledEnvWidthKnob("Env Width", envWidthKnob)
{
    setLookAndFeel(lookAndFeel);

    filterGroup.setText("Wah Filter");
    addAndMakeVisible(&filterGroup);
    lfoGroup.setText("LFO Modulation");
    addAndMakeVisible(&lfoGroup);
    envelopeGroup.setText("Envelope Modulation");
    addAndMakeVisible(&envelopeGroup);

    centreFreqKnob.setDoubleClickReturnValue(true, 350.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledCentreFreqKnob);
    filterQKnob.setDoubleClickReturnValue(true, 5.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledFilterQKnob);
    lfoFreqKnob.setDoubleClickReturnValue(true, 2.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLfoFreqKnob);
    lfoWidthKnob.setDoubleClickReturnValue(true, 1000.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLfoWidthKnob);
    attackKnob.setDoubleClickReturnValue(true, 0.005, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledAttackKnob);
    decayKnob.setDoubleClickReturnValue(true, 0.1, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledReleaseKnob);
    envWidthKnob.setDoubleClickReturnValue(true, 0.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledEnvWidthKnob);

    processor.parameters.attachControls(
        centreFreqKnob,
        filterQKnob,
        lfoFreqKnob,
        lfoWidthKnob,
        attackKnob,
        decayKnob,
        envWidthKnob );

    setSize (860, 180);
}

AutoWahEditor::~AutoWahEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void AutoWahEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    filterGroup.setBounds(bounds.removeFromLeft(2 * 100 + 3 * 10));
    auto widgetsArea = filterGroup.getBounds().reduced(10);
    widgetsArea.removeFromTop(20);
    labeledCentreFreqKnob.setBounds(widgetsArea.removeFromLeft(100));
    widgetsArea.removeFromLeft(10);
    labeledFilterQKnob.setBounds(widgetsArea.removeFromLeft(100));

    bounds.removeFromLeft(20);
    lfoGroup.setBounds(bounds.removeFromLeft(2 * 100 + 3 * 10));
    widgetsArea = lfoGroup.getBounds().reduced(10);
    widgetsArea.removeFromTop(20);
    labeledLfoFreqKnob.setBounds(widgetsArea.removeFromLeft(100));
    widgetsArea.removeFromLeft(10);
    labeledLfoWidthKnob.setBounds(widgetsArea.removeFromLeft(100));

    bounds.removeFromLeft(20);
    envelopeGroup.setBounds(bounds.removeFromLeft(3 * 100 + 4 * 10));
    widgetsArea = envelopeGroup.getBounds().reduced(10);
    widgetsArea.removeFromTop(20);
    labeledAttackKnob.setBounds(widgetsArea.removeFromLeft(100));
    widgetsArea.removeFromLeft(10);
    labeledReleaseKnob.setBounds(widgetsArea.removeFromLeft(100));
    widgetsArea.removeFromLeft(10);
    labeledEnvWidthKnob.setBounds(widgetsArea);
}

void AutoWahEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}