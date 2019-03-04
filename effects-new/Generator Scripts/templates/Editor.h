#pragma once
#include "JuceHeader.h"
#include "{{ projectName }}Processor.h"
#include "LabeledKnob.h"

class {{ projectName }}Editor  : public AudioProcessorEditor
{
public:
    {{ projectName }}Editor ({{ projectName }}Processor&);
    virtual ~{{ projectName }}Editor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    {{ projectName }}Processor& processor;

    GroupComponent mainGroup;

    {% for p in params %}
    {% if p['enumCount'] > 0 %}
    ComboBox {{ p['baseName'] }}Combo; Label {{ p['baseName'] }}Label;
    {% else %}
    BasicKnob {{ p['baseName'] }}Knob; LabeledKnob labeled{{ p['baseName']|capitalizeFirstLetterOnly }}Knob;
    {% endif %}
    {% endfor %}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR ({{ projectName }}Editor)
};
