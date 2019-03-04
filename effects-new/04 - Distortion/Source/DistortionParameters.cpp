#include "DistortionParameters.h"

// Names are symbolic names used to tag AudioParameter objects and XML attributes
// Labels are human-friendly identifiers for use in GUIs
const String DistortionParameters::distTypeID = "distType";
const String DistortionParameters::distTypeName = TRANS("Type");
const String DistortionParameters::distTypeLabel = "";
const String DistortionParameters::gainID = "gain";
const String DistortionParameters::gainName = TRANS("Gain");
const String DistortionParameters::gainLabel = "dB";

AudioProcessorValueTreeState::ParameterLayout DistortionParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterInt>(
        distTypeID, distTypeName,
        0, 5 - 1, 0,
        distTypeLabel,
        [](int value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getIntValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        gainID, gainName,
        NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f,
        gainLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

DistortionParameters::DistortionParameters(AudioProcessorValueTreeState& vts)
    : distType(Distortion::kHardClipping)
    , linearGain(1.0f)
    , valueTreeState(vts)
    , distTypeListener(distType)
    , gainListener(linearGain, -100.0f)
{
    valueTreeState.addParameterListener(distTypeID, &distTypeListener);
    valueTreeState.addParameterListener(gainID, &gainListener);
}

DistortionParameters::~DistortionParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(distTypeID, &distTypeListener);
    valueTreeState.removeParameterListener(gainID, &gainListener);
}

void DistortionParameters::detachControls()
{
    distTypeAttachment.reset(nullptr);
    gainAttachment.reset(nullptr);
}

void DistortionParameters::attachControls(ComboBox& distTypeCombo, Slider& gainKnob)
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    distTypeAttachment.reset(new CbAt(valueTreeState, distTypeID, distTypeCombo));
    gainAttachment.reset(new SlAt(valueTreeState, gainID, gainKnob));
}
