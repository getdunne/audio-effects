#include "GainParameters.h"

// Names are symbolic names used to tag AudioParameter objects and XML attributes
// Labels are human-friendly identifiers for use in GUIs
const String GainParameters::gainID = "gain";
const String GainParameters::gainName = TRANS("Gain");
const String GainParameters::gainLabel = TRANS("dB");

AudioProcessorValueTreeState::ParameterLayout GainParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        gainID, gainName,
        NormalisableRange<float>(-100.0f, 12.0f), 0.0f,
        gainLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

GainParameters::GainParameters(AudioProcessorValueTreeState& vts)
    : linearGain(1.0f)
    , valueTreeState(vts)
    , gainListener(linearGain, -100.0f)
{
    valueTreeState.addParameterListener(gainID, &gainListener);
}

GainParameters::~GainParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(gainID, &gainListener);
}

void GainParameters::detachControls()
{
    gainAttachment.reset(nullptr);
}

void GainParameters::attachControls(Slider& gainKnob)
{
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    gainAttachment.reset(new SlAt(valueTreeState, gainID, gainKnob));
}
