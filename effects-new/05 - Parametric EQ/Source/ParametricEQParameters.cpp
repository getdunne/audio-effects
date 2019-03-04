#include "ParametricEQParameters.h"

// Names are symbolic names used to tag AudioParameter objects and XML attributes
// Labels are human-friendly identifiers for use in GUIs
const String ParametricEQParameters::centreFreqID = "centreFreq";
const String ParametricEQParameters::centreFreqName = TRANS("Centre Freq");
const String ParametricEQParameters::centreFreqLabel = "Hz";
const String ParametricEQParameters::filterQID = "filterQ";
const String ParametricEQParameters::filterQName = TRANS("Q");
const String ParametricEQParameters::filterQLabel = "";
const String ParametricEQParameters::gainID = "gain";
const String ParametricEQParameters::gainName = TRANS("Gain");
const String ParametricEQParameters::gainLabel = "dB";

AudioProcessorValueTreeState::ParameterLayout ParametricEQParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        centreFreqID, centreFreqName,
        NormalisableRange<float>(10.0f, 20000.0f, 0.0f), 1000.0f,
        centreFreqLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        filterQID, filterQName,
        NormalisableRange<float>(0.1f, 20.0f, 0.1f), 2.0f,
        filterQLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        gainID, gainName,
        NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f,
        gainLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

ParametricEQParameters::ParametricEQParameters(AudioProcessorValueTreeState& vts,
                                               AudioProcessorValueTreeState::Listener* processor)
    : centreFreqHz(1000.0f)
    , filterQ(2.0f)
    , linearGain(1.0f)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , centreFreqListener(centreFreqHz)
    , filterQListener(filterQ)
    , gainListener(linearGain)
{
    valueTreeState.addParameterListener(centreFreqID, &centreFreqListener);
    valueTreeState.addParameterListener(filterQID, &filterQListener);
    valueTreeState.addParameterListener(gainID, &gainListener);

    // processor also needs to listen for parameter changes, in order to update filters
    valueTreeState.addParameterListener(centreFreqID, processorAsListener);
    valueTreeState.addParameterListener(filterQID, processorAsListener);
    valueTreeState.addParameterListener(gainID, processorAsListener);
}

ParametricEQParameters::~ParametricEQParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(centreFreqID, &centreFreqListener);
    valueTreeState.removeParameterListener(filterQID, &filterQListener);
    valueTreeState.removeParameterListener(gainID, &gainListener);
    valueTreeState.removeParameterListener(centreFreqID, processorAsListener);
    valueTreeState.removeParameterListener(filterQID, processorAsListener);
    valueTreeState.removeParameterListener(gainID, processorAsListener);
}

void ParametricEQParameters::detachControls()
{
    centreFreqAttachment.reset(nullptr);
    filterQAttachment.reset(nullptr);
    gainAttachment.reset(nullptr);
}

void ParametricEQParameters::attachControls(
    Slider& centreFreqKnob,
    Slider& filterQKnob,
    Slider& gainKnob )
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    centreFreqAttachment.reset(new SlAt(valueTreeState, centreFreqID, centreFreqKnob));
    filterQAttachment.reset(new SlAt(valueTreeState, filterQID, filterQKnob));
    gainAttachment.reset(new SlAt(valueTreeState, gainID, gainKnob));
}