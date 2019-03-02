#pragma once
#include "JuceHeader.h"

/*  One of the most serious omissions in the JUCE library is a standard "knob" control.
    JUCE offers a "rotary" style Slider, but this is not satisfactory. The BasicKnob
    and BasicLookAndFeel classes are based on techniques presented in the JUCE Tutorial
    https://docs.juce.com/master/tutorial_look_and_feel_customisation.html
*/

class BasicKnob : public Slider
{
    friend class BasicLookAndFeel;

public:
    BasicKnob(float minValue, float maxValue, const String& unit = {});
    virtual ~BasicKnob() = default;

    // Slider
    double getValueFromText(const String& text) override;
    String getTextFromValue(double value) override;

    // BasicKnob
    void setFillColour(Colour c) { fillColour = c; }
    void setOutlineColour(Colour c) { outlineColour = c; }
    void setPointerColour(Colour c) { pointerColour = c; }

protected:
    Colour fillColour, outlineColour, pointerColour;
    String unitOfMeasure;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicKnob)
};

class BasicLookAndFeel : public LookAndFeel_V4
{
public:
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle,
        Slider& slider) override;
};
