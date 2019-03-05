#include "AutoWahParameters.h"

// Names are symbolic names used to tag AudioParameter objects and XML attributes
// Labels are human-friendly identifiers for use in GUIs
const String AutoWahParameters::centreFreqID = "centreFreq";
const String AutoWahParameters::centreFreqName = TRANS("Centre Freq");
const String AutoWahParameters::centreFreqLabel = "Hz";
const String AutoWahParameters::filterQID = "filterQ";
const String AutoWahParameters::filterQName = TRANS("Q");
const String AutoWahParameters::filterQLabel = "";
const String AutoWahParameters::lfoFreqID = "lfoFreq";
const String AutoWahParameters::lfoFreqName = TRANS("LFO Freq");
const String AutoWahParameters::lfoFreqLabel = "Hz";
const String AutoWahParameters::lfoDepthID = "lfoDepth";
const String AutoWahParameters::lfoDepthName = TRANS("LFO Depth");
const String AutoWahParameters::lfoDepthLabel = "Hz";
const String AutoWahParameters::attackID = "attack";
const String AutoWahParameters::attackName = TRANS("Attack Time");
const String AutoWahParameters::attackLabel = "sec";
const String AutoWahParameters::releaseID = "release";
const String AutoWahParameters::releaseName = TRANS("Release Time");
const String AutoWahParameters::releaseLabel = "sec";
const String AutoWahParameters::modDepthID = "modDepth";
const String AutoWahParameters::modDepthName = TRANS("Env Depth");
const String AutoWahParameters::modDepthLabel = "%";

AudioProcessorValueTreeState::ParameterLayout AutoWahParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        centreFreqID, centreFreqName,
        NormalisableRange<float>(200.0f, 1000.0f, 0.0f), 350.0f,
        centreFreqLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        filterQID, filterQName,
        NormalisableRange<float>(2.0f, 20.0f, 0.1f), 5.0f,
        filterQLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        lfoFreqID, lfoFreqName,
        NormalisableRange<float>(0.2f, 20.0f, 0.0f), 2.0f,
        lfoFreqLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        lfoDepthID, lfoDepthName,
        NormalisableRange<float>(0.0f, 2000.0f, 0.0f), 1000.0f,
        lfoDepthLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        attackID, attackName,
        NormalisableRange<float>(0.0f, 0.1f, 0.0f), 0.005f,
        attackLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        releaseID, releaseName,
        NormalisableRange<float>(0.0f, 2.0f, 0.0f), 0.1f,
        releaseLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        modDepthID, modDepthName,
        NormalisableRange<float>(0.0f, 100.0f, 0.0f), 0.0f,
        modDepthLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

AutoWahParameters::AutoWahParameters(AudioProcessorValueTreeState& vts)
    : centreFreqHz(350.0f)
    , filterQ(5.0f)
    , lfoFreqHz(2.0f)
    , lfoDepth(1000.0f)
    , attackTimeMs(0.005f)
    , releaseTimeMs(0.1f)
    , envDepth(0.0f)
    , valueTreeState(vts)
    , centreFreqListener(centreFreqHz)
    , filterQListener(filterQ)
    , lfoFreqListener(lfoFreqHz)
    , lfoDepthListener(lfoDepth)
    , attackListener(attackTimeMs)
    , releaseListener(releaseTimeMs)
    , modDepthListener(envDepth)
{
    valueTreeState.addParameterListener(centreFreqID, &centreFreqListener);
    valueTreeState.addParameterListener(filterQID, &filterQListener);
    valueTreeState.addParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.addParameterListener(lfoDepthID, &lfoDepthListener);
    valueTreeState.addParameterListener(attackID, &attackListener);
    valueTreeState.addParameterListener(releaseID, &releaseListener);
    valueTreeState.addParameterListener(modDepthID, &modDepthListener);
}

AutoWahParameters::~AutoWahParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(centreFreqID, &centreFreqListener);
    valueTreeState.removeParameterListener(filterQID, &filterQListener);
    valueTreeState.removeParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.removeParameterListener(lfoDepthID, &lfoDepthListener);
    valueTreeState.removeParameterListener(attackID, &attackListener);
    valueTreeState.removeParameterListener(releaseID, &releaseListener);
    valueTreeState.removeParameterListener(modDepthID, &modDepthListener);
}

void AutoWahParameters::detachControls()
{
    centreFreqAttachment.reset(nullptr);
    filterQAttachment.reset(nullptr);
    lfoFreqAttachment.reset(nullptr);
    lfoDepthAttachment.reset(nullptr);
    attackAttachment.reset(nullptr);
    releaseAttachment.reset(nullptr);
    modDepthAttachment.reset(nullptr);
}

void AutoWahParameters::attachControls(
    Slider& centreFreqKnob,
    Slider& filterQKnob,
    Slider& lfoFreqKnob,
    Slider& lfoDepthKnob,
    Slider& attackKnob,
    Slider& releaseKnob,
    Slider& modDepthKnob )
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    centreFreqAttachment.reset(new SlAt(valueTreeState, centreFreqID, centreFreqKnob));
    filterQAttachment.reset(new SlAt(valueTreeState, filterQID, filterQKnob));
    lfoFreqAttachment.reset(new SlAt(valueTreeState, lfoFreqID, lfoFreqKnob));
    lfoDepthAttachment.reset(new SlAt(valueTreeState, lfoDepthID, lfoDepthKnob));
    attackAttachment.reset(new SlAt(valueTreeState, attackID, attackKnob));
    releaseAttachment.reset(new SlAt(valueTreeState, releaseID, releaseKnob));
    modDepthAttachment.reset(new SlAt(valueTreeState, modDepthID, modDepthKnob));
}