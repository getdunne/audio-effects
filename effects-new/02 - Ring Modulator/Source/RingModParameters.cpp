#include "RingModParameters.h"

// Names are symbolic names used to tag AudioParameter objects and XML attributes
// Labels are human-friendly identifiers for use in GUIs
const String RingModParameters::carrierFreqID = "carrierFreq";
const String RingModParameters::carrierFreqName = TRANS("Carrier Freq");
const String RingModParameters::carrierFreqLabel = TRANS("Hz");
const String RingModParameters::lfoWaveformID = "lfoWaveform";
const String RingModParameters::lfoWaveformName = TRANS("LFO Waveform");
const String RingModParameters::lfoWaveformLabel = "";
const String RingModParameters::lfoFreqID = "lfoFreq";
const String RingModParameters::lfoFreqName = TRANS("LFO Freq");
const String RingModParameters::lfoFreqLabel = TRANS("Hz");
const String RingModParameters::lfoWidthID = "lfoWidth";
const String RingModParameters::lfoWidthName = TRANS("LFO Sweep Width");
const String RingModParameters::lfoWidthLabel = "Hz";

AudioProcessorValueTreeState::ParameterLayout RingModParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        carrierFreqID, carrierFreqName,
        NormalisableRange<float>(1.0f, 1000.0f), 100.0f,
        carrierFreqLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    params.push_back(std::make_unique<AudioParameterInt>(
        lfoWaveformID, lfoWaveformName,
        0, int(RingModLFO::kNumWaveforms) - 1, int(RingModLFO::kWaveformSine),
        lfoWaveformLabel,
        [](int value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getIntValue(); }));

    params.push_back(std::make_unique<AudioParameterFloat>(
        lfoFreqID, lfoFreqName,
        NormalisableRange<float>(0.1f, 10.0f), 1.0f,
        lfoFreqLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    params.push_back(std::make_unique<AudioParameterFloat>(
        lfoWidthID, lfoWidthName,
        NormalisableRange<float>(0.0f, 100.0f), 0.0f,
        lfoWidthLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

RingModParameters::RingModParameters(AudioProcessorValueTreeState& vts)
    : carrierFreqHz(100.0f)
    , lfoWaveform(RingModLFO::kWaveformSine)
    , lfoFreqHz(1.0f)
    , lfoWidthHz(0.0f)
    , valueTreeState(vts)
    , carrierFreqListener(carrierFreqHz)
    , lfoWaveformListener(lfoWaveform)
    , lfoFreqListener(lfoFreqHz)
    , lfoWidthListener(lfoWidthHz)
{
    valueTreeState.addParameterListener(carrierFreqID, &carrierFreqListener);
    valueTreeState.addParameterListener(lfoWaveformID, &lfoWaveformListener);
    valueTreeState.addParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.addParameterListener(lfoWidthID, &lfoWidthListener);
}

RingModParameters::~RingModParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(carrierFreqID, &carrierFreqListener);
    valueTreeState.removeParameterListener(lfoWaveformID, &lfoWaveformListener);
    valueTreeState.removeParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.removeParameterListener(lfoWidthID, &lfoWidthListener);
}

void RingModParameters::detachControls()
{
    carrierFreqAttachment.reset(nullptr);
    lfoWaveformAttachment.reset(nullptr);
    lfoFreqAttachment.reset(nullptr);
    lfoWidthAttachment.reset(nullptr);
}

void RingModParameters::attachControls(Slider& carrierFreqKnob,
                                       ComboBox& lfoWaveformCombo, Slider& lfoFreqKnob, Slider& lfoWidthKnob)
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    lfoWaveformAttachment.reset(new CbAt(valueTreeState, lfoWaveformID, lfoWaveformCombo));
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    carrierFreqAttachment.reset(new SlAt(valueTreeState, carrierFreqID, carrierFreqKnob));
    lfoFreqAttachment.reset(new SlAt(valueTreeState, lfoFreqID, lfoFreqKnob));
    lfoWidthAttachment.reset(new SlAt(valueTreeState, lfoWidthID, lfoWidthKnob));
}
