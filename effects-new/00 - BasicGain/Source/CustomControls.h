#pragma once
#include "JuceHeader.h"
#include "GainParameters.h"

class BasicKnob : public Slider
{
    friend class BasicLookAndFeel;

public:
    BasicKnob(const String& labelText, float minValue, float maxValue);
    virtual ~BasicKnob() = default;

    // Component
    void resized() override;
    void mouseEnter(const MouseEvent&) override;
    void mouseExit(const MouseEvent&) override;
    void mouseMove(const MouseEvent&) override;

    // BasicKnob
    void setFillColour(Colour c) { fillColour = c; }
    void setOutlineColour(Colour c) { outlineColour = c; }
    void setPointerColour(Colour c) { pointerColour = c; }

protected:
    Colour fillColour, outlineColour, pointerColour;

    // geometry
    Rectangle<int> mouseBounds;

    class OpaqueLabel : public Label
    {
    public:
        OpaqueLabel(const String& labelText);
        virtual ~OpaqueLabel() = default;
        
        void paint(Graphics&) override;
    };
    OpaqueLabel label;
    
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

// Adapted from JUCE Tutorial: Control audio levels using decibels
// https://docs.juce.com/master/tutorial_synth_db_level_control.html
class DecibelGainKnob : public BasicKnob
{
public:
    DecibelGainKnob(const String& labelText, float minDB, float maxDB, float minusInfDB=-100.0f)
        : BasicKnob(labelText, minDB, maxDB)
        , minusInfinitydB(minusInfDB)
    {
    }
    virtual ~DecibelGainKnob() = default;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DecibelGainKnob)
};

class LabeledKnob : public Component, protected MouseListener
{
public:
    LabeledKnob();
    virtual ~LabeledKnob();

    void mouseEnter(const MouseEvent&) override;
    void mouseExit(const MouseEvent&) override;
    void mouseMove(const MouseEvent&) override;

protected:

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabeledKnob)
};
