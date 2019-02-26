#pragma once
#include "JuceHeader.h"
#include "GainProcessor.h"

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
};


class MyLookAndFeel : public LookAndFeel_V4
{
public:
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle,
        Slider& slider) override;
};


class GainEditor  : public AudioProcessorEditor, protected Timer
{
public:
    GainEditor (GainProcessor&);
    virtual ~GainEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

protected:
    void timerCallback() override;

private:
    SharedResourcePointer<MyLookAndFeel> lookAndFeel;
    GainProcessor& processor;

    MySlider gainSlider;

    TextButton undoButton, redoButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainEditor)
};
