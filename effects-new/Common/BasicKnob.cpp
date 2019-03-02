#include "BasicKnob.h"

BasicKnob::BasicKnob(float minValue, float maxValue, const String& unit)
    : Slider()
    , unitOfMeasure(unit)
{
    fillColour = Colours::steelblue;
    outlineColour = Colours::slategrey;
    pointerColour = Colours::lightblue;

    setTextBoxStyle(NoTextBox, false, 0, 0);
    setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    setRange(minValue, maxValue);
}

double BasicKnob::getValueFromText(const String& text)
{
    auto valueText = text.upToFirstOccurrenceOf(" ", false, false).trim();
    return valueText.getDoubleValue();
}

String BasicKnob::getTextFromValue(double value)
{
    String text = String(value);
    if (unitOfMeasure.isNotEmpty()) text += " " + unitOfMeasure;
    return text;
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
