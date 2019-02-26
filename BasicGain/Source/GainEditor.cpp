#include "GainProcessor.h"
#include "GainEditor.h"

MySlider::MySlider(float minValue, float maxValue)
    : Slider()
{
    fillColour = Colours::steelblue;
    outlineColour = Colours::slategrey;
    pointerColour = Colours::lightblue;

    setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    setPopupDisplayEnabled(true, true, 0);

    setRange(minValue, maxValue);
}

// Change the look of JUCE's "rotary sliders" so they're more like traditional knobs. This code is adapted
// from the example at https://www.juce.com/doc/tutorial_look_and_feel_customisation.
void MyLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle,
    Slider& slider)
{
    auto theSlider = dynamic_cast<MySlider*>(&slider);
    if (theSlider)
    {
        // basic geometry
        const float radius = jmin(width / 2, height / 2) - 10.0f;
        const float centreX = x + width * 0.5f;
        const float centreY = y + height * 0.5f;
        const float rx = centreX - radius;
        const float ry = centreY - radius;
        const float rw = radius * 2.0f;
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour(theSlider->fillColour);
        g.fillEllipse(rx, ry, rw, rw);

        // outline
        g.setColour(theSlider->outlineColour);
        g.drawEllipse(rx, ry, rw, rw, 1.0f);

        // pointer
        Path p;
        const float pointerLength = radius * 0.5f;
        const float pointerThickness = 2.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(theSlider->pointerColour);
        g.fillPath(p);
    }
    else LookAndFeel_V4::drawRotarySlider(g, x, y, width, height, sliderPos,
                                          rotaryStartAngle, rotaryEndAngle, slider);
}

GainEditor::GainEditor (GainProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , gainSlider(0.0f, 2.0f)
{
    setLookAndFeel(lookAndFeel);

    addAndMakeVisible(gainSlider);
    gainSlider.setFillColour(Colours::darkred);

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
