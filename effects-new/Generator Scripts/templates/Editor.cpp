#include "{{ projectName }}Processor.h"
#include "{{ projectName }}Editor.h"

{{ projectName }}Editor::{{ projectName }}Editor ({{ projectName }}Processor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    {% for p in params %}
    {% if p['enumCount'] == 0 %}
    , {{ p['baseName'] }}Knob({{ projectName }}Parameters::{{ p['baseName'] }}Min, {{ projectName }}Parameters::{{ p['baseName'] }}Max, {{ projectName }}Parameters::{{ p['baseName'] }}Label)
    , labeled{{ p['baseName']|capitalizeFirstLetterOnly }}Knob({{ projectName }}Parameters::{{ p['baseName'] }}Name, {{ p['baseName'] }}Knob)
    {% endif %}
    {% endfor %}
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("{{ projectName }}");
    addAndMakeVisible(&mainGroup);

    {% for p in params %}
    {% if p['enumCount'] > 0 %}
    {{ p['baseName'] }}Label.setText("{{ p['labelText'] }}", dontSendNotification);
    {{ p['baseName'] }}Label.setJustificationType(Justification::right);
    addAndMakeVisible(&{{ p['baseName'] }}Label);
    {{ p['baseName'] }}Combo.setEditableText(false);
    {{ p['baseName'] }}Combo.setJustificationType(Justification::centredLeft);
    ENUMCLASS::populateComboBox({{ p['baseName'] }}Combo);
    addAndMakeVisible({{ p['baseName'] }}Combo);
    {% else %}
    {{ p['baseName'] }}Knob.setDoubleClickReturnValue(true, double({{ projectName }}Parameters::{{ p['baseName'] }}Default), ModifierKeys::noModifiers);
    addAndMakeVisible(labeled{{ p['baseName']|capitalizeFirstLetterOnly }}Knob);
    {% endif %}
    {% endfor %}

    processor.parameters.attachControls(
        {% for p in params[:-1] %}
        {% if p['enumCount'] > 0 %}
        {{ p['baseName'] }}Combo,
        {% else %}
        {{ p['baseName'] }}Knob,
        {% endif %}
        {% endfor %}
        {% if params[-1]['enumCount'] > 0 %}
        {{ params[-1]['baseName'] }}Combo );
        {% else %}
        {{ params[-1]['baseName'] }}Knob );
        {% endif %}

    setSize (60 + 100 * {{ params|count }} + 10 * ({{ params|count }} - 1), 180);
}

{{ projectName }}Editor::~{{ projectName }}Editor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void {{ projectName }}Editor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    mainGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);
    widgetsArea.removeFromTop(20);
    int width = (widgetsArea.getWidth() - ({{ params|count }} - 1) * 10) / {{ params|count }};
    {% for p in params[:-1] %}
    labeled{{ p['baseName']|capitalizeFirstLetterOnly }}Knob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    {% endfor %}
    labeled{{ params[-1]['baseName']|capitalizeFirstLetterOnly }}Knob.setBounds(widgetsArea);
}

void {{ projectName }}Editor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}
