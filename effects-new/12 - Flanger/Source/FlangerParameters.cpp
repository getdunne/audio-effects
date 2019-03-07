#include "FlangerParameters.h"

// Min Delay
const String FlangerParameters::minDelayID = "minDelay";
const String FlangerParameters::minDelayName = TRANS("Min Delay");
const String FlangerParameters::minDelayLabel = "ms";
const float FlangerParameters::minDelayMin = 1.0f;
const float FlangerParameters::minDelayMax = 20.0f;
const float FlangerParameters::minDelayDefault = 2.5f;
const float FlangerParameters::minDelayStep = 0.5f;
// Sweep Width
const String FlangerParameters::sweepWidthID = "sweepWidth";
const String FlangerParameters::sweepWidthName = TRANS("Sweep Width");
const String FlangerParameters::sweepWidthLabel = "ms";
const float FlangerParameters::sweepWidthMin = 1.0f;
const float FlangerParameters::sweepWidthMax = 20.0f;
const float FlangerParameters::sweepWidthDefault = 10.0f;
const float FlangerParameters::sweepWidthStep = 0.5f;
// Depth
const String FlangerParameters::depthID = "depth";
const String FlangerParameters::depthName = TRANS("Depth");
const String FlangerParameters::depthLabel = "%";
const float FlangerParameters::depthMin = 0.0f;
const float FlangerParameters::depthMax = 100.0f;
const float FlangerParameters::depthDefault = 100.0f;
const float FlangerParameters::depthStep = 1.0f;
// Feedback
const String FlangerParameters::feedbackID = "feedback";
const String FlangerParameters::feedbackName = TRANS("Feedback");
const String FlangerParameters::feedbackLabel = "%";
const float FlangerParameters::feedbackMin = 0.0f;
const float FlangerParameters::feedbackMax = 50.0f;
const float FlangerParameters::feedbackDefault = 0.0f;
const float FlangerParameters::feedbackStep = 1.0f;
// LFO Frequency
const String FlangerParameters::lfoFreqID = "lfoFreq";
const String FlangerParameters::lfoFreqName = TRANS("LFO Freq");
const String FlangerParameters::lfoFreqLabel = "Hz";
const float FlangerParameters::lfoFreqMin = 0.05f;
const float FlangerParameters::lfoFreqMax = 2.0f;
const float FlangerParameters::lfoFreqDefault = 0.2f;
const float FlangerParameters::lfoFreqStep = 0.01f;
// Stereo
const String FlangerParameters::stereoID = "stereo";
const String FlangerParameters::stereoName = TRANS("Stereo");
const String FlangerParameters::stereoLabel = "";
const int FlangerParameters::stereoEnumCount = 2;
const int FlangerParameters::stereoDefault = 0;
// LFO Waveform
const String FlangerParameters::lfoWaveformID = "lfoWaveform";
const String FlangerParameters::lfoWaveformName = TRANS("LFO");
const String FlangerParameters::lfoWaveformLabel = "";
const int FlangerParameters::lfoWaveformEnumCount = 4;
const int FlangerParameters::lfoWaveformDefault = 0;
// Interpolation Type
const String FlangerParameters::interpTypeID = "interpType";
const String FlangerParameters::interpTypeName = TRANS("Interpolation Type");
const String FlangerParameters::interpTypeLabel = "";
const int FlangerParameters::interpTypeEnumCount = 3;
const int FlangerParameters::interpTypeDefault = 1;

AudioProcessorValueTreeState::ParameterLayout FlangerParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        minDelayID, minDelayName,
        NormalisableRange<float>(minDelayMin, minDelayMax, minDelayStep), minDelayDefault,
        minDelayLabel,
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
    params.push_back(std::make_unique<AudioParameterFloat>(
        depthID, depthName,
        NormalisableRange<float>(depthMin, depthMax, depthStep), depthDefault,
        depthLabel,
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
        lfoFreqID, lfoFreqName,
        NormalisableRange<float>(lfoFreqMin, lfoFreqMax, lfoFreqStep), lfoFreqDefault,
        lfoFreqLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterInt>(
        stereoID, stereoName,
        0, stereoEnumCount - 1, stereoDefault,
        stereoLabel,
        [](int value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getIntValue(); }));
    params.push_back(std::make_unique<AudioParameterInt>(
        lfoWaveformID, lfoWaveformName,
        0, lfoWaveformEnumCount - 1, lfoWaveformDefault,
        lfoWaveformLabel,
        [](int value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getIntValue(); }));
    params.push_back(std::make_unique<AudioParameterInt>(
        interpTypeID, interpTypeName,
        0, interpTypeEnumCount - 1, interpTypeDefault,
        interpTypeLabel,
        [](int value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getIntValue(); }));

    return { params.begin(), params.end() };
}

FlangerParameters::FlangerParameters(AudioProcessorValueTreeState& vts)
    : minDelayMs(minDelayDefault)
    , sweepWidthMs(sweepWidthDefault)
    , depth(depthDefault)
    , feedback(feedbackDefault)
    , lfoFreqHz(lfoFreqDefault)
    , stereoMode(stereoDefault)
    , lfoWaveform((FlangerLFO::Waveform)lfoWaveformDefault)
    , interpolationType((FlangerInterpolation::Type)interpTypeDefault)
    , valueTreeState(vts)
    , minDelayListener(minDelayMs)
    , sweepWidthListener(sweepWidthMs)
    , depthListener(depth, 0.01f)
    , feedbackListener(feedback, 0.01f)
    , lfoFreqListener(lfoFreqHz)
    , stereoListener(stereoMode)
    , lfoWaveformListener(lfoWaveform)
    , interpTypeListener(interpolationType)
{
    valueTreeState.addParameterListener(minDelayID, &minDelayListener);
    valueTreeState.addParameterListener(sweepWidthID, &sweepWidthListener);
    valueTreeState.addParameterListener(depthID, &depthListener);
    valueTreeState.addParameterListener(feedbackID, &feedbackListener);
    valueTreeState.addParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.addParameterListener(stereoID, &stereoListener);
    valueTreeState.addParameterListener(lfoWaveformID, &lfoWaveformListener);
    valueTreeState.addParameterListener(interpTypeID, &interpTypeListener);

    depth *= 0.01f;
    feedback *= 0.01f;
}

FlangerParameters::~FlangerParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(minDelayID, &minDelayListener);
    valueTreeState.removeParameterListener(sweepWidthID, &sweepWidthListener);
    valueTreeState.removeParameterListener(depthID, &depthListener);
    valueTreeState.removeParameterListener(feedbackID, &feedbackListener);
    valueTreeState.removeParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.removeParameterListener(stereoID, &stereoListener);
    valueTreeState.removeParameterListener(lfoWaveformID, &lfoWaveformListener);
    valueTreeState.removeParameterListener(interpTypeID, &interpTypeListener);
}

void FlangerParameters::detachControls()
{
    minDelayAttachment.reset(nullptr);
    sweepWidthAttachment.reset(nullptr);
    depthAttachment.reset(nullptr);
    feedbackAttachment.reset(nullptr);
    lfoFreqAttachment.reset(nullptr);
    stereoAttachment.reset(nullptr);
    lfoWaveformAttachment.reset(nullptr);
    interpTypeAttachment.reset(nullptr);
}

void FlangerParameters::attachControls(
    Slider& minDelayKnob,
    Slider& sweepWidthKnob,
    Slider& depthKnob,
    Slider& feedbackKnob,
    Slider& lfoFreqKnob,
    ToggleButton& stereoButton,
    ComboBox& lfoWaveformCombo,
    ComboBox& interpTypeCombo )
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using TbAt = AudioProcessorValueTreeState::ButtonAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    minDelayAttachment.reset(new SlAt(valueTreeState, minDelayID, minDelayKnob));
    sweepWidthAttachment.reset(new SlAt(valueTreeState, sweepWidthID, sweepWidthKnob));
    depthAttachment.reset(new SlAt(valueTreeState, depthID, depthKnob));
    feedbackAttachment.reset(new SlAt(valueTreeState, feedbackID, feedbackKnob));
    lfoFreqAttachment.reset(new SlAt(valueTreeState, lfoFreqID, lfoFreqKnob));
    stereoAttachment.reset(new TbAt(valueTreeState, stereoID, stereoButton));
    lfoWaveformAttachment.reset(new CbAt(valueTreeState, lfoWaveformID, lfoWaveformCombo));
    interpTypeAttachment.reset(new CbAt(valueTreeState, interpTypeID, interpTypeCombo));
}