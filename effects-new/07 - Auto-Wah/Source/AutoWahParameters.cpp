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
const String AutoWahParameters::lfoWidthID = "lfoWidth";
const String AutoWahParameters::lfoWidthName = TRANS("LFO Width");
const String AutoWahParameters::lfoWidthLabel = "Hz";
const String AutoWahParameters::attackID = "attack";
const String AutoWahParameters::attackName = TRANS("Attack Time");
const String AutoWahParameters::attackLabel = "sec";
const String AutoWahParameters::decayID = "decay";
const String AutoWahParameters::decayName = TRANS("Decay Time");
const String AutoWahParameters::decayLabel = "sec";
const String AutoWahParameters::envWidthID = "modDepth";
const String AutoWahParameters::envWidthName = TRANS("Env Width");
const String AutoWahParameters::envWidthLabel = "Hz";

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
        lfoWidthID, lfoWidthName,
        NormalisableRange<float>(0.0f, 2000.0f, 0.0f), 1000.0f,
        lfoWidthLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        attackID, attackName,
        NormalisableRange<float>(0.0f, 0.1f, 0.0f), 0.006f,
        attackLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        decayID, decayName,
        NormalisableRange<float>(0.0f, 2.0f, 0.0f), 0.2f,
        decayLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        envWidthID, envWidthName,
        NormalisableRange<float>(0.0f, 4000.0f, 0.0f), 0.0f,
        envWidthLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

AutoWahParameters::AutoWahParameters(AudioProcessorValueTreeState& vts,
    AudioProcessorValueTreeState::Listener* processor)
    : centreFreqHz(350.0f)
    , filterQ(5.0f)
    , lfoFreqHz(2.0f)
    , lfoWidthHz(1000.0f)
    , attackTimeSec(0.006f)
    , decayTimeSec(0.2f)
    , envWidthHz(0.0f)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , centreFreqListener(centreFreqHz)
    , filterQListener(filterQ)
    , lfoFreqListener(lfoFreqHz)
    , lfoWidthListener(lfoWidthHz)
    , attackListener(attackTimeSec)
    , releaseListener(decayTimeSec)
    , envWidthListener(envWidthHz)
{
    valueTreeState.addParameterListener(centreFreqID, &centreFreqListener);
    valueTreeState.addParameterListener(filterQID, &filterQListener);
    valueTreeState.addParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.addParameterListener(lfoWidthID, &lfoWidthListener);
    valueTreeState.addParameterListener(attackID, &attackListener);
    valueTreeState.addParameterListener(decayID, &releaseListener);
    valueTreeState.addParameterListener(envWidthID, &envWidthListener);
    valueTreeState.addParameterListener(centreFreqID, processorAsListener);
    valueTreeState.addParameterListener(filterQID, processorAsListener);
    valueTreeState.addParameterListener(attackID, processorAsListener);
    valueTreeState.addParameterListener(decayID, processorAsListener);
}

AutoWahParameters::~AutoWahParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(centreFreqID, &centreFreqListener);
    valueTreeState.removeParameterListener(filterQID, &filterQListener);
    valueTreeState.removeParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.removeParameterListener(lfoWidthID, &lfoWidthListener);
    valueTreeState.removeParameterListener(attackID, &attackListener);
    valueTreeState.removeParameterListener(decayID, &releaseListener);
    valueTreeState.removeParameterListener(envWidthID, &envWidthListener);
    valueTreeState.removeParameterListener(centreFreqID, processorAsListener);
    valueTreeState.removeParameterListener(filterQID, processorAsListener);
    valueTreeState.removeParameterListener(attackID, processorAsListener);
    valueTreeState.removeParameterListener(decayID, processorAsListener);
}

void AutoWahParameters::detachControls()
{
    centreFreqAttachment.reset(nullptr);
    filterQAttachment.reset(nullptr);
    lfoFreqAttachment.reset(nullptr);
    lfoWidthAttachment.reset(nullptr);
    attackAttachment.reset(nullptr);
    releaseAttachment.reset(nullptr);
    envWidthAttachment.reset(nullptr);
}

void AutoWahParameters::attachControls(
    Slider& centreFreqKnob,
    Slider& filterQKnob,
    Slider& lfoFreqKnob,
    Slider& lfoWidthKnob,
    Slider& attackKnob,
    Slider& decayKnob,
    Slider& envWidthKnob )
{
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    centreFreqAttachment.reset(new SlAt(valueTreeState, centreFreqID, centreFreqKnob));
    filterQAttachment.reset(new SlAt(valueTreeState, filterQID, filterQKnob));
    lfoFreqAttachment.reset(new SlAt(valueTreeState, lfoFreqID, lfoFreqKnob));
    lfoWidthAttachment.reset(new SlAt(valueTreeState, lfoWidthID, lfoWidthKnob));
    attackAttachment.reset(new SlAt(valueTreeState, attackID, attackKnob));
    releaseAttachment.reset(new SlAt(valueTreeState, decayID, decayKnob));
    envWidthAttachment.reset(new SlAt(valueTreeState, envWidthID, envWidthKnob));
}