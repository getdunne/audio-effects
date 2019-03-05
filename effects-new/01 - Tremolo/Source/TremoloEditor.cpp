#include "TremoloProcessor.h"
#include "TremoloEditor.h"
#include "TremoloLFO.h"

TremoloEditor::TremoloEditor (TremoloProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , lfoFreqKnob(0.1f, 10.0f, "Hz")
    , labeledLfoFreqKnob("LFO Freq", lfoFreqKnob)
    , modDepthKnob(0.0f, 100.0f, "%")
    , labeledModDepthKnob("Depth", modDepthKnob)
{
    setLookAndFeel(lookAndFeel);

    tremoloGroup.setText("Tremolo");
    addAndMakeVisible(&tremoloGroup);

    lfoWaveformLabel.setText("LFO", dontSendNotification);
    lfoWaveformLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&lfoWaveformLabel);

    lfoWaveformCombo.setEditableText(false);
    lfoWaveformCombo.setJustificationType(Justification::centredLeft);
    TremoloLFO::populateWaveformComboBox(lfoWaveformCombo);
    addAndMakeVisible(lfoWaveformCombo);

    lfoFreqKnob.setDoubleClickReturnValue(true, 1.0f, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLfoFreqKnob);

    modDepthKnob.setDoubleClickReturnValue(true, 0.5f, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledModDepthKnob);

    processor.parameters.attachControls(lfoWaveformCombo, lfoFreqKnob, modDepthKnob);

    setSize (320, 250);
}

TremoloEditor::~TremoloEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void TremoloEditor::resized()
{
    auto groupArea = getLocalBounds().reduced(20);
    tremoloGroup.setBounds(groupArea);

    auto widgetsArea = groupArea.reduced(20);
    widgetsArea.removeFromTop(12);
    auto waveformArea = widgetsArea.removeFromTop(24);
    lfoWaveformLabel.setBounds(waveformArea.removeFromLeft(40));
    lfoWaveformCombo.setBounds(waveformArea);
    widgetsArea.removeFromTop(10);

    int knobWidth = widgetsArea.getWidth() / 2 - 10;
    labeledLfoFreqKnob.setBounds(widgetsArea.removeFromLeft(knobWidth));
    labeledModDepthKnob.setBounds(widgetsArea.removeFromRight(knobWidth));
}

void TremoloEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}
