#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class {{ projectName }}Parameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    {% for p in params %}
    static const String {{ p['baseName'] }}ID, {{ p['baseName'] }}Name, {{ p['baseName'] }}Label;
    {% if p['enumCount'] > 0 %}
    static const int {{ p['baseName'] }}EnumCount, {{ p['baseName'] }}Default;
    {% else %}
    static const float {{ p['baseName'] }}Min, {{ p['baseName'] }}Max, {{ p['baseName'] }}Default, {{ p['baseName'] }}Step;
    {% endif %}
    {% endfor %}
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    {{ projectName }}Parameters(AudioProcessorValueTreeState& vts);
    ~{{ projectName }}Parameters();

    void detachControls();
    void attachControls(
        {% for p in params[:-1] %}
        {% if p['enumCount'] > 0 %}
        ComboBox& {{ p['baseName'] }}Combo,
        {% else %}
        Slider& {{ p['baseName'] }}Knob,
        {% endif %}
        {% endfor %}
        {% if params[-1]['enumCount'] > 0 %}
        ComboBox& {{ params[-1]['baseName'] }}Combo );
        {% else %}
        Slider& {{ params[-1]['baseName'] }}Knob );
        {% endif %}

    // working parameter values
    {% for p in params %}
    {% if p['enumCount'] > 0 %}
    ENUMTYPE {{ p['workingVar'] }};
    {% else %}
    float {{ p['workingVar'] }};
    {% endif %}
    {% endfor %}

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Attachment objects link GUI controls to parameters
    {% for p in params %}
    {% if p['enumCount'] > 0 %}
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> {{ p['baseName'] }}Attachment;
    {% else %}
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> {{ p['baseName'] }}Attachment;
    {% endif %}
    {% endfor %}

    // Listener objects link parameters to working variables
    {% for p in params %}
    {% if p['enumCount'] > 0 %}
    EnumListener<ENUMTYPE> {{ p['baseName'] }}Listener;
    {% else %}
    FloatListener {{ p['baseName'] }}Listener;
    {% endif %}
    {% endfor %}
};
