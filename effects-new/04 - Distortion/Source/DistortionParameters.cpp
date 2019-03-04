#include "DistortionParameters.h"

// Names are symbolic names used to tag AudioParameter objects and XML attributes
// Labels are human-friendly identifiers for use in GUIs
const String DistortionParameters::gainID = "gain";
const String DistortionParameters::gainName = TRANS("Gain");
const String DistortionParameters::gainLabel = TRANS("dB");
const String DistortionParameters::typeID = "type";
const String DistortionParameters::typeName = TRANS("Type");
const String DistortionParameters::typeLabel = TRANS("");

AudioProcessorValueTreeState::ParameterLayout DistortionParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        gainID, gainName,
        NormalisableRange<float>(-100.0f, 12.0f), 0.0f,
        gainLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    
    params.push_back(std::make_unique<AudioParameterInt>(
        typeID, typeName,
        0, int(DistortionType::_numTypes) - 1, int(DistortionType::_hardClipping),
        typeLabel,
        [](int value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getIntValue(); }));

    return { params.begin(), params.end() };
}

DistortionParameters::DistortionParameters(AudioProcessorValueTreeState& vts)
    : linearGain(1.0f)
    , type(DistortionType::_hardClipping)
    , valueTreeState(vts)
    , gainListener(linearGain, -100.0f)
    , typeListener(type)
{
    valueTreeState.addParameterListener(gainID, &gainListener);
    valueTreeState.addParameterListener(typeID, &typeListener);
}

DistortionParameters::~DistortionParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(gainID, &gainListener);
    valueTreeState.removeParameterListener(typeID, &typeListener);
}

void DistortionParameters::detachControls()
{
    gainAttachment.reset(nullptr);
    typeAttachment.reset(nullptr);
}

void DistortionParameters::attachControls(Slider& gainKnob, ComboBox& typeCombo)
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    typeAttachment.reset(new CbAt(valueTreeState, typeID, typeCombo));
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    gainAttachment.reset(new SlAt(valueTreeState, gainID, gainKnob));
}
