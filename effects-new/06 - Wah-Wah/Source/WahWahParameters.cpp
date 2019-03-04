#include "WahWahParameters.h"

// Names are symbolic names used to tag AudioParameter objects and XML attributes
// Labels are human-friendly identifiers for use in GUIs
const String WahWahParameters::centreFreqID = "centreFreq";
const String WahWahParameters::centreFreqName = TRANS("Centre Freq");
const String WahWahParameters::centreFreqLabel = "Hz";
const String WahWahParameters::filterQID = "filterQ";
const String WahWahParameters::filterQName = TRANS("Q");
const String WahWahParameters::filterQLabel = "";

AudioProcessorValueTreeState::ParameterLayout WahWahParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        centreFreqID, centreFreqName,
        NormalisableRange<float>(400.0f, 2000.0f, 0.0f), 400.0f,
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

    return { params.begin(), params.end() };
}

WahWahParameters::WahWahParameters(AudioProcessorValueTreeState& vts,
                                   AudioProcessorValueTreeState::Listener* processor)
    : centreFreqHz(400.0f)
    , filterQ(5.0f)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , centreFreqListener(centreFreqHz)
    , filterQListener(filterQ)
{
    valueTreeState.addParameterListener(centreFreqID, &centreFreqListener);
    valueTreeState.addParameterListener(filterQID, &filterQListener);
    valueTreeState.addParameterListener(centreFreqID, processorAsListener);
    valueTreeState.addParameterListener(filterQID, processorAsListener);
}

WahWahParameters::~WahWahParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(centreFreqID, &centreFreqListener);
    valueTreeState.removeParameterListener(filterQID, &filterQListener);
    valueTreeState.removeParameterListener(centreFreqID, processorAsListener);
    valueTreeState.removeParameterListener(filterQID, processorAsListener);
}

void WahWahParameters::detachControls()
{
    centreFreqAttachment.reset(nullptr);
    filterQAttachment.reset(nullptr);
}

void WahWahParameters::attachControls(
    Slider& centreFreqKnob,
    Slider& filterQKnob )
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    centreFreqAttachment.reset(new SlAt(valueTreeState, centreFreqID, centreFreqKnob));
    filterQAttachment.reset(new SlAt(valueTreeState, filterQID, filterQKnob));
}