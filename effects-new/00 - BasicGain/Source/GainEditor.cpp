#include "GainProcessor.h"
#include "GainEditor.h"

GainEditor::GainEditor (GainProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , gainKnob(-100.0f, 12.0f)
    , labeledGainKnob("Gain", gainKnob)
{
    setLookAndFeel(lookAndFeel);

    undoGroup.setText("Undo Management");
    addAndMakeVisible(&undoGroup);

    undoButton.setButtonText(TRANS("Undo"));
    undoButton.onClick = [this]() { processor.undoManager.undo(); };
    addAndMakeVisible(undoButton);

    redoButton.setButtonText(TRANS("Redo"));
    redoButton.onClick = [this]() { processor.undoManager.redo(); };
    addAndMakeVisible(redoButton);

    gainGroup.setText("Gain Control");
    addAndMakeVisible(&gainGroup);

    gainKnob.setDoubleClickReturnValue(true, 0.0, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledGainKnob);

    processor.parameters.attachControls(gainKnob);
    processor.undoManager.clearUndoHistory();

    // For Undo: execute timer callback once immediately, then every 500ms thereafter
    timerCallback();
    startTimer(500);

    setSize (300, 260);
}

GainEditor::~GainEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void GainEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    auto undoArea = bounds.removeFromTop(80);
    undoGroup.setBounds(undoArea);
    auto undoButtonsArea = undoArea.reduced(20);
    undoButtonsArea.removeFromTop(10);
    auto buttonWidth = (undoButtonsArea.getWidth() - 20) / 2;
    undoButton.setBounds(undoButtonsArea.removeFromLeft(buttonWidth));
    redoButton.setBounds(undoButtonsArea.removeFromRight(buttonWidth));

    bounds.removeFromTop(10);
    gainGroup.setBounds(bounds);
    auto gainArea = bounds.reduced(10);
    gainArea.removeFromTop(10);
    labeledGainKnob.setBounds(gainArea);
}

void GainEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}

void GainEditor::timerCallback()
{
    // Doing this on a timed basis is copied from the JUCE ValueTreesDemo, but we don't want to start
    // new transactions in the middle of an update gesture like dragging a slider, so we only do this
    // when the mouse button is not down.

    if (!isMouseButtonDownAnywhere())
    {
        processor.undoManager.beginNewTransaction();
        undoButton.setEnabled(processor.undoManager.canUndo());
        redoButton.setEnabled(processor.undoManager.canRedo());
    }
}
