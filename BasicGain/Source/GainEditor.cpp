#include "GainProcessor.h"
#include "GainEditor.h"

GainEditor::GainEditor (GainProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , gainSlider(-100.0f, 12.0f)
{
    setLookAndFeel(lookAndFeel);

    addAndMakeVisible(gainSlider);
    gainSlider.setDoubleClickReturnValue(true, -6.0, ModifierKeys::noModifiers);
    gainSlider.setFillColour(Colours::darkblue);

    undoButton.setButtonText(TRANS("Undo"));
    undoButton.onClick = [this]() { processor.undoManager.undo(); };
    addAndMakeVisible(undoButton);

    redoButton.setButtonText(TRANS("Redo"));
    redoButton.onClick = [this]() { processor.undoManager.redo(); };
    addAndMakeVisible(redoButton);

    processor.parameters.attachControls(gainSlider);
    processor.undoManager.clearUndoHistory();

    // execute timer callback once immediately, then every 500ms thereafter
    timerCallback();
    startTimer(500);

    setSize (400, 300);
}

GainEditor::~GainEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void GainEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    auto undoButtonStrip = bounds.removeFromTop(24);
    auto buttonWidth = (undoButtonStrip.getWidth() - 20) / 2;
    undoButton.setBounds(undoButtonStrip.removeFromLeft(buttonWidth));
    redoButton.setBounds(undoButtonStrip.removeFromRight(buttonWidth));
    gainSlider.setBounds(bounds.withSizeKeepingCentre(100, 100));
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
