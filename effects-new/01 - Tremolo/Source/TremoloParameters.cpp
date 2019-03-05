#include "TremoloParameters.h"

// Names are symbolic names used to tag AudioParameter objects and XML attributes
// Labels are human-friendly identifiers for use in GUIs
const String TremoloParameters::lfoWaveformID = "lfoWaveform";
const String TremoloParameters::lfoWaveformName = TRANS("LFO Waveform");
const String TremoloParameters::lfoWaveformLabel = "";
const String TremoloParameters::lfoFreqID = "lfoFreq";
const String TremoloParameters::lfoFreqName = TRANS("LFO Freq");
const String TremoloParameters::lfoFreqLabel = TRANS("Hz");
const String TremoloParameters::modDepthID = "modDepth";
const String TremoloParameters::modDepthName = TRANS("Mod Depth");
const String TremoloParameters::modDepthLabel = "%";

AudioProcessorValueTreeState::ParameterLayout TremoloParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterInt>(
        lfoWaveformID, lfoWaveformName,
        0, int(TremoloLFO::kNumWaveforms) - 1, int(TremoloLFO::kWaveformSine),
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
        modDepthID, modDepthName,
        NormalisableRange<float>(0.0f, 100.0f), 50.0f,
        modDepthLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

TremoloParameters::TremoloParameters(AudioProcessorValueTreeState& vts)
    : lfoWaveform(TremoloLFO::kWaveformSine)
    , lfoFreqHz(1.0f)
    , modDepth(0.5f)
    , valueTreeState(vts)
    , lfoWaveformListener(lfoWaveform)
    , lfoFreqListener(lfoFreqHz)
    , modDepthListener(modDepth, 0.01f)
{
    valueTreeState.addParameterListener(lfoWaveformID, &lfoWaveformListener);
    valueTreeState.addParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.addParameterListener(modDepthID, &modDepthListener);
}

TremoloParameters::~TremoloParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(lfoWaveformID, &lfoWaveformListener);
    valueTreeState.removeParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.removeParameterListener(modDepthID, &modDepthListener);
}

void TremoloParameters::detachControls()
{
    lfoWaveformAttachment.reset(nullptr);
    lfoFreqAttachment.reset(nullptr);
    modDepthAttachment.reset(nullptr);
}

void TremoloParameters::attachControls(ComboBox& lfoWaveformCombo, Slider& lfoFreqKnob, Slider& modDepthKnob)
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    lfoWaveformAttachment.reset(new CbAt(valueTreeState, lfoWaveformID, lfoWaveformCombo));
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    lfoFreqAttachment.reset(new SlAt(valueTreeState, lfoFreqID, lfoFreqKnob));
    modDepthAttachment.reset(new SlAt(valueTreeState, modDepthID, modDepthKnob));
}
