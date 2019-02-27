#pragma once
#include "JuceHeader.h"

class MySlider : public Slider
{
    friend class MyLookAndFeel;

public:
    MySlider(float minValue, float maxValue);
    virtual ~MySlider() = default;

    void setFillColour(Colour c) { fillColour = c; }
    void setOutlineColour(Colour c) { outlineColour = c; }
    void setPointerColour(Colour c) { pointerColour = c; }

protected:
    Colour fillColour, outlineColour, pointerColour;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MySlider)
};

class MyLookAndFeel : public LookAndFeel_V4
{
public:
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle,
        Slider& slider) override;
};

// Adapted from JUCE Tutorial: Control audio levels using decibels
// https://docs.juce.com/master/tutorial_synth_db_level_control.html
class DecibelGainSlider : public MySlider
{
public:
    DecibelGainSlider(float minDB, float maxDB, float minusInfDB=-100.0f)
        : MySlider(minDB, maxDB)
        , minusInfinitydB(minusInfDB)
    {
    }

    double getValueFromText(const String& text) override
    {
        auto decibelText = text.upToFirstOccurrenceOf("dB", false, false).trim();
        return decibelText.equalsIgnoreCase("-INF") ? minusInfinitydB
            : decibelText.getDoubleValue();
    }

    String getTextFromValue(double value) override
    {
        return Decibels::toString(value);
    }

private:
    const float minusInfinitydB;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DecibelGainSlider)
};
