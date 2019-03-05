#include "RingModProcessor.h"
#include "RingModEditor.h"
#include "RingModLFO.h"

RingModEditor::RingModEditor(RingModProcessor& p)
    : AudioProcessorEditor(&p)
    , processor(p)
    , carrierFreqKnob(1.0f, 1000.0f, "Hz")
    , labeledCarrierFreqKnob("Carrier Freq", carrierFreqKnob)
    , lfoFreqKnob(0.1f, 10.0f, "Hz")
    , labeledLfoFreqKnob("LFO Freq", lfoFreqKnob)
    , lfoWidthKnob(0.0f, 100.0f, "Hz")
    , labeledLfoWidthKnob("Sweep Width", lfoWidthKnob)
{
    setLookAndFeel(lookAndFeel);

    carrierGroup.setText("Carrier");
    addAndMakeVisible(&carrierGroup);

    carrierFreqKnob.setDoubleClickReturnValue(true, 100.0f, ModifierKeys::noModifiers);
    addAndMakeVisible(&labeledCarrierFreqKnob);

    lfoGroup.setText("LFO");
    addAndMakeVisible(&lfoGroup);

    lfoWaveformLabel.setText("Waveform", dontSendNotification);
    lfoWaveformLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&lfoWaveformLabel);

    lfoWaveformCombo.setEditableText(false);
    lfoWaveformCombo.setJustificationType(Justification::centredLeft);
    RingModLFO::populateWaveformComboBox(lfoWaveformCombo);
    addAndMakeVisible(lfoWaveformCombo);

    lfoFreqKnob.setDoubleClickReturnValue(true, 1.0f, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLfoFreqKnob);

    lfoWidthKnob.setDoubleClickReturnValue(true, 0.0f, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLfoWidthKnob);

    processor.parameters.attachControls(carrierFreqKnob, lfoWaveformCombo, lfoFreqKnob, lfoWidthKnob);

    setSize (480, 250);
}

RingModEditor::~RingModEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void RingModEditor::resized()
{
    auto groupsArea = getLocalBounds().reduced(20);

    carrierGroup.setBounds(groupsArea.removeFromLeft(140));
    groupsArea.removeFromLeft(20);
    lfoGroup.setBounds(groupsArea);

    auto widgetsArea = carrierGroup.getBounds().reduced(20);
    widgetsArea.removeFromTop(46);
    labeledCarrierFreqKnob.setBounds(widgetsArea);

    widgetsArea = lfoGroup.getBounds().reduced(20);
    widgetsArea.removeFromTop(12);
    auto waveformArea = widgetsArea.removeFromTop(24);
    lfoWaveformLabel.setBounds(waveformArea.removeFromLeft(80));
    lfoWaveformCombo.setBounds(waveformArea);
    widgetsArea.removeFromTop(10);

    int knobWidth = widgetsArea.getWidth() / 2 - 10;
    labeledLfoFreqKnob.setBounds(widgetsArea.removeFromLeft(knobWidth));
    labeledLfoWidthKnob.setBounds(widgetsArea.removeFromRight(knobWidth));
}

void RingModEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}
