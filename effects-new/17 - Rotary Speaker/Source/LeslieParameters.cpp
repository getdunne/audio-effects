#include "LeslieParameters.h"

// Speed
const String LeslieParameters::speedID = "speed";
const String LeslieParameters::speedName = TRANS("Speed");
const String LeslieParameters::speedLabel = "";
const float LeslieParameters::speedMin = 0.0f;
const float LeslieParameters::speedMax = 8.0f;
const float LeslieParameters::speedDefault = 4.0f;
const float LeslieParameters::speedStep = 1.0f;

AudioProcessorValueTreeState::ParameterLayout LeslieParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        speedID, speedName,
        NormalisableRange<float>(speedMin, speedMax, speedStep), speedDefault,
        speedLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

LeslieParameters::LeslieParameters(AudioProcessorValueTreeState& vts,
                                   AudioProcessorValueTreeState::Listener* processor)
    : speed(speedDefault)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , speedListener(speed)
{
    valueTreeState.addParameterListener(speedID, processorAsListener);
    valueTreeState.addParameterListener(speedID, &speedListener);
}

LeslieParameters::~LeslieParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(speedID, processorAsListener);
    valueTreeState.removeParameterListener(speedID, &speedListener);
}

void LeslieParameters::detachControls()
{
    speedAttachment.reset(nullptr);
}

void LeslieParameters::attachControls(Slider& speedKnob)
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    speedAttachment.reset(new SlAt(valueTreeState, speedID, speedKnob));
}
