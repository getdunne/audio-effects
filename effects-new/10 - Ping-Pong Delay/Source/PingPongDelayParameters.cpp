#include "PingPongDelayParameters.h"

// Left Delay Time
const String PingPongDelayParameters::delayTimeLID = "delayTimeL";
const String PingPongDelayParameters::delayTimeLName = TRANS("L-R Delay Time");
const String PingPongDelayParameters::delayTimeLLabel = "sec";
const float PingPongDelayParameters::delayTimeLMin = 0.01f;
const float PingPongDelayParameters::delayTimeLMax = 2.0f;
const float PingPongDelayParameters::delayTimeLDefault = 0.5f;
const float PingPongDelayParameters::delayTimeLStep = 0.01f;
// Right Delay Time
const String PingPongDelayParameters::delayTimeRID = "delayTimeR";
const String PingPongDelayParameters::delayTimeRName = TRANS("R-L Delay Time");
const String PingPongDelayParameters::delayTimeRLabel = "sec";
const float PingPongDelayParameters::delayTimeRMin = 0.01f;
const float PingPongDelayParameters::delayTimeRMax = 2.0f;
const float PingPongDelayParameters::delayTimeRDefault = 0.5f;
const float PingPongDelayParameters::delayTimeRStep = 0.01f;
// Feedback
const String PingPongDelayParameters::feedbackID = "feedback";
const String PingPongDelayParameters::feedbackName = TRANS("Feedback");
const String PingPongDelayParameters::feedbackLabel = "%";
const float PingPongDelayParameters::feedbackMin = 0.0f;
const float PingPongDelayParameters::feedbackMax = 99.5f;
const float PingPongDelayParameters::feedbackDefault = 75.0f;
const float PingPongDelayParameters::feedbackStep = 0.5f;
// Wet Mix Level
const String PingPongDelayParameters::wetLevelID = "wetLevel";
const String PingPongDelayParameters::wetLevelName = TRANS("Delay Mix Level");
const String PingPongDelayParameters::wetLevelLabel = "%";
const float PingPongDelayParameters::wetLevelMin = 0.0f;
const float PingPongDelayParameters::wetLevelMax = 100.0f;
const float PingPongDelayParameters::wetLevelDefault = 50.0f;
const float PingPongDelayParameters::wetLevelStep = 1.0f;

AudioProcessorValueTreeState::ParameterLayout PingPongDelayParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        delayTimeLID, delayTimeLName,
        NormalisableRange<float>(delayTimeLMin, delayTimeLMax, delayTimeLStep), delayTimeLDefault,
        delayTimeLLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        delayTimeRID, delayTimeRName,
        NormalisableRange<float>(delayTimeRMin, delayTimeRMax, delayTimeRStep), delayTimeRDefault,
        delayTimeRLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        feedbackID, feedbackName,
        NormalisableRange<float>(feedbackMin, feedbackMax, feedbackStep), feedbackDefault,
        feedbackLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        wetLevelID, wetLevelName,
        NormalisableRange<float>(wetLevelMin, wetLevelMax, wetLevelStep), wetLevelDefault,
        wetLevelLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

PingPongDelayParameters::PingPongDelayParameters(AudioProcessorValueTreeState& vts,
                                                 AudioProcessorValueTreeState::Listener* processor)
    : delaySecL(delayTimeLDefault)
    , delaySecR(delayTimeRDefault)
    , feedback(feedbackDefault)
    , wetLevel(wetLevelDefault)
    , reverseChannels(false)
    , linkDelays(false)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , delayTimeLListener(delaySecL)
    , delayTimeRListener(delaySecR)
    , feedbackListener(feedback, 0.01f)
    , wetLevelListener(wetLevel, 0.01f)
{
    valueTreeState.addParameterListener(delayTimeLID, &delayTimeLListener);
    valueTreeState.addParameterListener(delayTimeRID, &delayTimeRListener);
    valueTreeState.addParameterListener(feedbackID, &feedbackListener);
    valueTreeState.addParameterListener(wetLevelID, &wetLevelListener);
    valueTreeState.addParameterListener(delayTimeLID, processorAsListener);
    valueTreeState.addParameterListener(delayTimeRID, processorAsListener);

    feedback *= 0.01f;
    wetLevel *= 0.01f;
}

PingPongDelayParameters::~PingPongDelayParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(delayTimeLID, &delayTimeLListener);
    valueTreeState.removeParameterListener(delayTimeRID, &delayTimeRListener);
    valueTreeState.removeParameterListener(feedbackID, &feedbackListener);
    valueTreeState.removeParameterListener(wetLevelID, &wetLevelListener);
    valueTreeState.removeParameterListener(delayTimeLID, processorAsListener);
    valueTreeState.removeParameterListener(delayTimeRID, processorAsListener);
}

void PingPongDelayParameters::detachControls()
{
    delayTimeLAttachment.reset(nullptr);
    delayTimeRAttachment.reset(nullptr);
    feedbackAttachment.reset(nullptr);
    wetLevelAttachment.reset(nullptr);
}

void PingPongDelayParameters::attachControls(
    Slider& delayTimeLKnob,
    Slider& delayTimeRKnob,
    Slider& feedbackKnob,
    Slider& wetLevelKnob )
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    delayTimeLAttachment.reset(new SlAt(valueTreeState, delayTimeLID, delayTimeLKnob));
    delayTimeRAttachment.reset(new SlAt(valueTreeState, delayTimeRID, delayTimeRKnob));
    feedbackAttachment.reset(new SlAt(valueTreeState, feedbackID, feedbackKnob));
    wetLevelAttachment.reset(new SlAt(valueTreeState, wetLevelID, wetLevelKnob));
}
