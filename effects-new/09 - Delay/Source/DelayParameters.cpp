#include "DelayParameters.h"

// Delay Time
const String DelayParameters::delayTimeID = "delayTime";
const String DelayParameters::delayTimeName = TRANS("Delay Time");
const String DelayParameters::delayTimeLabel = "sec";
const float DelayParameters::delayTimeMin = 0.01f;
const float DelayParameters::delayTimeMax = 2.0f;
const float DelayParameters::delayTimeDefault = 0.5f;
const float DelayParameters::delayTimeStep = 0.01f;
// Feedback
const String DelayParameters::feedbackID = "feedback";
const String DelayParameters::feedbackName = TRANS("Feedback");
const String DelayParameters::feedbackLabel = "%";
const float DelayParameters::feedbackMin = 0.0f;
const float DelayParameters::feedbackMax = 99.5f;
const float DelayParameters::feedbackDefault = 75.0f;
const float DelayParameters::feedbackStep = 0.5f;
// Dry Mix Level
const String DelayParameters::dryLevelID = "dryLevel";
const String DelayParameters::dryLevelName = TRANS("Dry Mix Level");
const String DelayParameters::dryLevelLabel = "%";
const float DelayParameters::dryLevelMin = 0.0f;
const float DelayParameters::dryLevelMax = 100.0f;
const float DelayParameters::dryLevelDefault = 100.0f;
const float DelayParameters::dryLevelStep = 1.0f;
// Wet Mix Level
const String DelayParameters::wetLevelID = "wetLevel";
const String DelayParameters::wetLevelName = TRANS("Delay Mix Level");
const String DelayParameters::wetLevelLabel = "%";
const float DelayParameters::wetLevelMin = 0.0f;
const float DelayParameters::wetLevelMax = 100.0f;
const float DelayParameters::wetLevelDefault = 50.0f;
const float DelayParameters::wetLevelStep = 1.0f;

AudioProcessorValueTreeState::ParameterLayout DelayParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        delayTimeID, delayTimeName,
        NormalisableRange<float>(delayTimeMin, delayTimeMax, delayTimeStep), delayTimeDefault,
        delayTimeLabel,
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
        dryLevelID, dryLevelName,
        NormalisableRange<float>(dryLevelMin, dryLevelMax, dryLevelStep), dryLevelDefault,
        dryLevelLabel,
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

DelayParameters::DelayParameters(AudioProcessorValueTreeState& vts,
                                 AudioProcessorValueTreeState::Listener* processor)
    : delaySec(delayTimeDefault)
    , feedback(feedbackDefault)
    , dryLevel(dryLevelDefault)
    , wetLevel(wetLevelDefault)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , delayTimeListener(delaySec)
    , feedbackListener(feedback, 0.01f)
    , dryLevelListener(dryLevel, 0.01f)
    , wetLevelListener(wetLevel, 0.01f)
{
    valueTreeState.addParameterListener(delayTimeID, &delayTimeListener);
    valueTreeState.addParameterListener(feedbackID, &feedbackListener);
    valueTreeState.addParameterListener(dryLevelID, &dryLevelListener);
    valueTreeState.addParameterListener(wetLevelID, &wetLevelListener);
    valueTreeState.addParameterListener(delayTimeID, processorAsListener);

    feedback *= 0.01f;
    dryLevel *= 0.01f;
    wetLevel *= 0.01f;
}

DelayParameters::~DelayParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(delayTimeID, &delayTimeListener);
    valueTreeState.removeParameterListener(feedbackID, &feedbackListener);
    valueTreeState.removeParameterListener(dryLevelID, &dryLevelListener);
    valueTreeState.removeParameterListener(wetLevelID, &wetLevelListener);
    valueTreeState.removeParameterListener(delayTimeID, processorAsListener);
}

void DelayParameters::detachControls()
{
    delayTimeAttachment.reset(nullptr);
    feedbackAttachment.reset(nullptr);
    dryLevelAttachment.reset(nullptr);
    wetLevelAttachment.reset(nullptr);
}

void DelayParameters::attachControls(
    Slider& delayTimeKnob,
    Slider& feedbackKnob,
    Slider& dryLevelKnob,
    Slider& wetLevelKnob )
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    delayTimeAttachment.reset(new SlAt(valueTreeState, delayTimeID, delayTimeKnob));
    feedbackAttachment.reset(new SlAt(valueTreeState, feedbackID, feedbackKnob));
    dryLevelAttachment.reset(new SlAt(valueTreeState, dryLevelID, dryLevelKnob));
    wetLevelAttachment.reset(new SlAt(valueTreeState, wetLevelID, wetLevelKnob));
}