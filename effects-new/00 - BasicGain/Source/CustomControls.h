#pragma once
#include "JuceHeader.h"
#include "GainParameters.h"

class BasicKnob : public Slider
{
    friend class BasicLookAndFeel;

public:
    BasicKnob(float minValue, float maxValue);
    virtual ~BasicKnob() = default;

    void setFillColour(Colour c) { fillColour = c; }
    void setOutlineColour(Colour c) { outlineColour = c; }
    void setPointerColour(Colour c) { pointerColour = c; }

protected:
    Colour fillColour, outlineColour, pointerColour;

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
    DecibelGainKnob(float minDB, float maxDB, float minusInfDB=-100.0f)
        : BasicKnob(minDB, maxDB)
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
        if (value == 0.0) return "0.0 dB";
        return Decibels::toString(value);
    }

private:
    const float minusInfinitydB;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DecibelGainKnob)
};

class LabeledKnob : public Component, protected Timer
{
    friend class LKMouseListener;

public:
    LabeledKnob(const String& lblText, BasicKnob& theKnob);
    virtual ~LabeledKnob();

    void resized() override;

protected:
    void timerCallback() override;
    void clearMouseOvers();
    void updateMouseOvers(const MouseEvent&);
    void updateLabel(const MouseEvent&);

private:
    BasicKnob& knob;
    Label label;
    String labelText;
    bool mouseOverKnob, mouseOverLabel;

    class LKMouseListener : public MouseListener
    {
    public:
        LKMouseListener(LabeledKnob& lk) : owner(lk) {}

        void mouseEnter(const MouseEvent&) override;
        void mouseExit(const MouseEvent&) override;
        void mouseMove(const MouseEvent&) override;

    private:
        LabeledKnob& owner;
    };
    LKMouseListener mouseListener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabeledKnob)
};
