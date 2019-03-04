#include "{{ projectName }}Parameters.h"

// Names are symbolic names used to tag AudioParameter objects and XML attributes
// Labels are human-friendly identifiers for use in GUIs
{% for p in params %}
const String {{ projectName }}Parameters::{{ p['baseName'] }}ID = "{{ p['baseName'] }}";
const String {{ projectName }}Parameters::{{ p['baseName'] }}Name = TRANS("{{ p['labelText'] }}");
const String {{ projectName }}Parameters::{{ p['baseName'] }}Label = "{{ p['unitOfMeasure'] }}";
{% endfor %}

AudioProcessorValueTreeState::ParameterLayout {{ projectName }}Parameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    {% for p in params %}
    {% if p['enumCount'] > 0 %}
    params.push_back(std::make_unique<AudioParameterInt>(
        {{ p['baseName'] }}ID, {{ p['baseName'] }}Name,
        0, {{ p['enumCount'] }} - 1, 0,
        {{ p['baseName'] }}Label,
        [](int value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getIntValue(); }));
    {% else %}
    params.push_back(std::make_unique<AudioParameterFloat>(
        {{ p['baseName'] }}ID, {{ p['baseName'] }}Name,
        NormalisableRange<float>({{ p['minValue']|makeFloat }}, {{ p['maxValue']|makeFloat }}, {{ p['stepValue']|makeFloat }}), {{ p['defaultValue']|makeFloat }},
        {{ p['baseName'] }}Label,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    {% endif %}
    {% endfor %}

    return { params.begin(), params.end() };
}

{{ projectName }}Parameters::{{ projectName }}Parameters(AudioProcessorValueTreeState& vts)
    : {{ params[0]['workingVar'] }}({{ params[0]['defaultValue']|makeFloat }})
    {% for p in params[1:] %}
    , {{ p['workingVar'] }}({{ p['defaultValue']|makeFloat }})
    {% endfor %}
    , valueTreeState(vts)
    {% for p in params %}
    , {{ p['baseName'] }}Listener({{ p['workingVar'] }})
    {% endfor %}
{
    {% for p in params %}
    valueTreeState.addParameterListener({{ p['baseName'] }}ID, &{{ p['baseName'] }}Listener);
    {% endfor %}
}

{{ projectName }}Parameters::~{{ projectName }}Parameters()
{
    detachControls();
    {% for p in params %}
    valueTreeState.removeParameterListener({{ p['baseName'] }}ID, &{{ p['baseName'] }}Listener);
    {% endfor %}
}

void {{ projectName }}Parameters::detachControls()
{
    {% for p in params %}
    {{ p['baseName'] }}Attachment.reset(nullptr);
    {% endfor %}
}

void {{ projectName }}Parameters::attachControls(
    {% for p in params[:-1] %}
    {% if p['enumCount'] > 0 %}
    ComboBox& {{ p['baseName'] }}Combo,
    {% else %}
    Slider& {{ p['baseName'] }}Knob,
    {% endif %}
    {% endfor %}
    {% if params[-1]['enumCount'] > 0 %}
    ComboBox& {{ params[-1]['baseName'] }}Combo )
    {% else %}
    Slider& {{ params[-1]['baseName'] }}Knob )
    {% endif %}
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    {% for p in params %}
    {% if p['enumCount'] > 0 %}
    {{ p['baseName'] }}Attachment.reset(new CbAt(valueTreeState, {{ p['baseName'] }}ID, {{ p['baseName'] }}Combo));
    {% else %}
    {{ p['baseName'] }}Attachment.reset(new SlAt(valueTreeState, {{ p['baseName'] }}ID, {{ p['baseName'] }}Knob));
    {% endif %}
    {% endfor %}
}
