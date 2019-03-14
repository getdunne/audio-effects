#include "CompressorParameters.h"

// Names are symbolic names used to tag AudioParameter objects and XML attributes
// Labels are human-friendly identifiers for use in GUIs
const String CompressorParameters::ratioID = "ratio";
const String CompressorParameters::ratioName = TRANS("Ratio");
const String CompressorParameters::ratioLabel = "";
const String CompressorParameters::attackID = "attack";
const String CompressorParameters::attackName = TRANS("Attack Time");
const String CompressorParameters::attackLabel = TRANS("ms");
const String CompressorParameters::releaseID = "release";
const String CompressorParameters::releaseName = TRANS("Release Time");
const String CompressorParameters::releaseLabel = TRANS("ms");
const String CompressorParameters::thresholdID = "threshold";
const String CompressorParameters::thresholdName = TRANS("Threshold");
const String CompressorParameters::thresholdLabel = TRANS("dB");
const String CompressorParameters::gainID = "gain";
const String CompressorParameters::gainName = TRANS("Make-Up Gain");
const String CompressorParameters::gainLabel = TRANS("dB");

AudioProcessorValueTreeState::ParameterLayout CompressorParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        ratioID, ratioName,
        NormalisableRange<float>(1.0f, 20.0f, 0.1f), 1.0f,
        ratioLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    params.push_back(std::make_unique<AudioParameterFloat>(
        attackID, attackName,
        NormalisableRange<float>(0.1f, 80.0f, 0.1f), 15.0f,
        attackLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    params.push_back(std::make_unique<AudioParameterFloat>(
        releaseID, releaseName,
        NormalisableRange<float>(1.0f, 1000.0f, 0.1f), 100.0f,
        releaseLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    params.push_back(std::make_unique<AudioParameterFloat>(
        thresholdID, thresholdName,
        NormalisableRange<float>(-60.0f, 0.0f, 0.5f), 0.0f,
        thresholdLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    params.push_back(std::make_unique<AudioParameterFloat>(
        gainID, gainName,
        NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f,
        gainLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

CompressorParameters::CompressorParameters(AudioProcessorValueTreeState& vts)
    : compressionRatio(1.0f)
    , attackTimeMs(15.0f), releaseTimeMs(100.0f)
    , threshold(0.0f)
    , makeUpGain(0.0f)
    , valueTreeState(vts)
    , ratioListener(compressionRatio)
    , attackListener(attackTimeMs)
    , releaseListener(releaseTimeMs)
    , thresholdListener(threshold)
    , gainListener(makeUpGain)
{
    valueTreeState.addParameterListener(ratioID, &ratioListener);
    valueTreeState.addParameterListener(attackID, &attackListener);
    valueTreeState.addParameterListener(releaseID, &releaseListener);
    valueTreeState.addParameterListener(thresholdID, &thresholdListener);
    valueTreeState.addParameterListener(gainID, &gainListener);
}

CompressorParameters::~CompressorParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(ratioID, &ratioListener);
    valueTreeState.removeParameterListener(attackID, &attackListener);
    valueTreeState.removeParameterListener(releaseID, &releaseListener);
    valueTreeState.removeParameterListener(thresholdID, &thresholdListener);
    valueTreeState.removeParameterListener(gainID, &gainListener);
}

void CompressorParameters::detachControls()
{
    ratioAttachment.reset(nullptr);
    attackAttachment.reset(nullptr);
    releaseAttachment.reset(nullptr);
    thresholdAttachment.reset(nullptr);
    gainAttachment.reset(nullptr);
}

void CompressorParameters::attachControls(Slider& ratioKnob, Slider& attackKnob, Slider& releaseKnob,
                                          Slider& thresholdKnob, Slider& gainKnob)
{
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    ratioAttachment.reset(new SlAt(valueTreeState, ratioID, ratioKnob));
    attackAttachment.reset(new SlAt(valueTreeState, attackID, attackKnob));
    releaseAttachment.reset(new SlAt(valueTreeState, releaseID, releaseKnob));
    thresholdAttachment.reset(new SlAt(valueTreeState, thresholdID, thresholdKnob));
    gainAttachment.reset(new SlAt(valueTreeState, gainID, gainKnob));
}
