#include "CustomControls.h"

BasicKnob::BasicKnob(float minValue, float maxValue)
    : Slider()
{
    fillColour = Colours::steelblue;
    outlineColour = Colours::slategrey;
    pointerColour = Colours::lightblue;

    setTextBoxStyle(NoTextBox, false, 0, 0);
    setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    setRange(minValue, maxValue);
}

LabeledKnob::LabeledKnob(const String& lblText, BasicKnob& theKnob)
    : knob(theKnob)
    , labelText(lblText)
    , mouseOverKnob(false)
    , mouseOverLabel(false)
    , mouseListener(*this)
{
    addAndMakeVisible(&knob);
    label.setText(labelText, dontSendNotification);
    label.setJustificationType(Justification::centred);
    label.setEditable(false, true, true);
    addAndMakeVisible(&label);
    addMouseListener(&mouseListener, true);

    knob.onValueChange = [this]()
    {
        if (mouseOverKnob || mouseOverLabel)
        {
            String vs = knob.getTextFromValue(knob.getValue());
            label.setText(vs, dontSendNotification);
            startTimer(3000);
        }
    };

    label.onTextChange = [this]()
    {
        knob.setValue(knob.getValueFromText(label.getText()));
    };
}

LabeledKnob::~LabeledKnob()
{
    removeMouseListener(&mouseListener);
}

void LabeledKnob::resized()
{
    auto bounds = getLocalBounds();
    label.setBounds(bounds.removeFromBottom(20));
    knob.setBounds(bounds);
}

void LabeledKnob::timerCallback()
{
    if (!mouseOverKnob && !mouseOverLabel && !label.isBeingEdited())
    {
        stopTimer();
        label.setText(labelText, dontSendNotification);
    }
}

void LabeledKnob::clearMouseOvers()
{
    mouseOverKnob = false;
    mouseOverLabel = false;
}

void LabeledKnob::updateMouseOvers(const MouseEvent& evt)
{
    MouseEvent mevt = evt.getEventRelativeTo(this);
    int knobDiameter = jmin(knob.getWidth(), knob.getHeight()) - 20;
    Rectangle<int> knobRect = knob.getBounds().withSizeKeepingCentre(knobDiameter, knobDiameter);
    knobRect.setHeight(knobDiameter + 10);
    mouseOverKnob = knobRect.contains(mevt.getPosition());
    mouseOverLabel = label.getBounds().contains(mevt.getPosition());
}

void LabeledKnob::updateLabel(const MouseEvent& evt)
{
    updateMouseOvers(evt);
    if (label.isBeingEdited()) return;

    if (mouseOverKnob || mouseOverLabel)
    {
        label.setText(knob.getTextFromValue(knob.getValue()), dontSendNotification);
    }
    else startTimer(3000);
}

void LabeledKnob::LKMouseListener::mouseEnter(const MouseEvent& evt)
{
    owner.updateLabel(evt);
}

void LabeledKnob::LKMouseListener::mouseExit(const MouseEvent&)
{
    owner.clearMouseOvers();
    owner.timerCallback();
}

void LabeledKnob::LKMouseListener::mouseMove(const MouseEvent& evt)
{
    owner.updateLabel(evt);
}


// Change the look of JUCE's "rotary sliders" so they're more like traditional knobs. This code is adapted
// from the example at https://www.juce.com/doc/tutorial_look_and_feel_customisation.
void BasicLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle,
    Slider& slider)
{
    auto theSlider = dynamic_cast<BasicKnob*>(&slider);
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
