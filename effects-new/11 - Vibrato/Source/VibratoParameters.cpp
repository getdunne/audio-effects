#include "VibratoParameters.h"

// LFO Waveform
const String VibratoParameters::lfoWaveformID = "lfoWaveform";
const String VibratoParameters::lfoWaveformName = TRANS("LFO");
const String VibratoParameters::lfoWaveformLabel = "";
const int VibratoParameters::lfoWaveformEnumCount = 5;
const int VibratoParameters::lfoWaveformDefault = 0;
// LFO Frequency
const String VibratoParameters::lfoFreqID = "lfoFreq";
const String VibratoParameters::lfoFreqName = TRANS("LFO Freq");
const String VibratoParameters::lfoFreqLabel = "Hz";
const float VibratoParameters::lfoFreqMin = 0.5f;
const float VibratoParameters::lfoFreqMax = 5.0f;
const float VibratoParameters::lfoFreqDefault = 2.0f;
const float VibratoParameters::lfoFreqStep = 0.025f;
// Sweep Width
const String VibratoParameters::sweepWidthID = "sweepWidth";
const String VibratoParameters::sweepWidthName = TRANS("Sweep Width");
const String VibratoParameters::sweepWidthLabel = "sec";
const float VibratoParameters::sweepWidthMin = 0.001f;
const float VibratoParameters::sweepWidthMax = 0.05f;
const float VibratoParameters::sweepWidthDefault = 0.01f;
const float VibratoParameters::sweepWidthStep = 0.0005f;
// Interpolation Type
const String VibratoParameters::interpTypeID = "interpType";
const String VibratoParameters::interpTypeName = TRANS("Interpolation Type");
const String VibratoParameters::interpTypeLabel = "";
const int VibratoParameters::interpTypeEnumCount = 3;
const int VibratoParameters::interpTypeDefault = 1;

AudioProcessorValueTreeState::ParameterLayout VibratoParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterInt>(
        lfoWaveformID, lfoWaveformName,
        0, lfoWaveformEnumCount - 1, lfoWaveformDefault,
        lfoWaveformLabel,
        [](int value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getIntValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        lfoFreqID, lfoFreqName,
        NormalisableRange<float>(lfoFreqMin, lfoFreqMax, lfoFreqStep), lfoFreqDefault,
        lfoFreqLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        sweepWidthID, sweepWidthName,
        NormalisableRange<float>(sweepWidthMin, sweepWidthMax, sweepWidthStep), sweepWidthDefault,
        sweepWidthLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterInt>(
        interpTypeID, interpTypeName,
        0, interpTypeEnumCount - 1, interpTypeDefault,
        interpTypeLabel,
        [](int value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getIntValue(); }));

    return { params.begin(), params.end() };
}

VibratoParameters::VibratoParameters(AudioProcessorValueTreeState& vts,
                                     AudioProcessorValueTreeState::Listener* processor)
    : lfoWaveform(VibratoLFO::Waveform(lfoWaveformDefault))
    , lfoFreqHz(lfoFreqDefault)
    , sweepWidthHz(sweepWidthDefault)
    , interpolationType(VibratoInterpolation::Type(interpTypeDefault))
    , valueTreeState(vts)
    , processorAsListener(processor)
    , lfoWaveformListener(lfoWaveform)
    , lfoFreqListener(lfoFreqHz)
    , sweepWidthListener(sweepWidthHz)
    , interpTypeListener(interpolationType)
{
    // ParameterListeners seem to get called in the reverse order to how they are added.
    // We need our processor to get notified of parameter changes AFTER our specialized
    // listeners have had a chance to update the working variables, so we add the
    // processorAsListener FIRST.
    valueTreeState.addParameterListener(lfoFreqID, processorAsListener);
    valueTreeState.addParameterListener(sweepWidthID, processorAsListener);
    valueTreeState.addParameterListener(lfoWaveformID, processorAsListener);

    valueTreeState.addParameterListener(interpTypeID, &interpTypeListener);
    valueTreeState.addParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.addParameterListener(sweepWidthID, &sweepWidthListener);
    valueTreeState.addParameterListener(lfoWaveformID, &lfoWaveformListener);
}

VibratoParameters::~VibratoParameters()
{
    detachControls();

    valueTreeState.removeParameterListener(lfoFreqID, processorAsListener);
    valueTreeState.removeParameterListener(sweepWidthID, processorAsListener);
    valueTreeState.removeParameterListener(lfoWaveformID, processorAsListener);

    valueTreeState.removeParameterListener(interpTypeID, &interpTypeListener);
    valueTreeState.removeParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.removeParameterListener(sweepWidthID, &sweepWidthListener);
    valueTreeState.removeParameterListener(lfoWaveformID, &lfoWaveformListener);
}

void VibratoParameters::detachControls()
{
    lfoWaveformAttachment.reset(nullptr);
    lfoFreqAttachment.reset(nullptr);
    sweepWidthAttachment.reset(nullptr);
    interpTypeAttachment.reset(nullptr);
}

void VibratoParameters::attachControls(
    ComboBox& lfoWaveformCombo,
    Slider& lfoFreqKnob,
    Slider& sweepWidthKnob,
    ComboBox& interpTypeCombo )
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    lfoFreqAttachment.reset(new SlAt(valueTreeState, lfoFreqID, lfoFreqKnob));
    sweepWidthAttachment.reset(new SlAt(valueTreeState, sweepWidthID, sweepWidthKnob));
    interpTypeAttachment.reset(new CbAt(valueTreeState, interpTypeID, interpTypeCombo));
    lfoWaveformAttachment.reset(new CbAt(valueTreeState, lfoWaveformID, lfoWaveformCombo));
}