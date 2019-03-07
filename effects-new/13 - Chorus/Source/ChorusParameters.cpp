#include "ChorusParameters.h"

// Min Delay
const String ChorusParameters::minDelayID = "minDelay";
const String ChorusParameters::minDelayName = TRANS("Min Delay");
const String ChorusParameters::minDelayLabel = "ms";
const float ChorusParameters::minDelayMin = 10.0f;
const float ChorusParameters::minDelayMax = 50.0f;
const float ChorusParameters::minDelayDefault = 30.0f;
const float ChorusParameters::minDelayStep = 1.0f;
// Sweep Width
const String ChorusParameters::sweepWidthID = "sweepWidth";
const String ChorusParameters::sweepWidthName = TRANS("Sweep Width");
const String ChorusParameters::sweepWidthLabel = "ms";
const float ChorusParameters::sweepWidthMin = 10.0f;
const float ChorusParameters::sweepWidthMax = 50.0f;
const float ChorusParameters::sweepWidthDefault = 20.0f;
const float ChorusParameters::sweepWidthStep = 1.0f;
// Depth
const String ChorusParameters::depthID = "depth";
const String ChorusParameters::depthName = TRANS("Depth");
const String ChorusParameters::depthLabel = "%";
const float ChorusParameters::depthMin = 0.0f;
const float ChorusParameters::depthMax = 100.0f;
const float ChorusParameters::depthDefault = 100.0f;
const float ChorusParameters::depthStep = 1.0f;
// Feedback
const String ChorusParameters::voiceCountID = "voiceCount";
const String ChorusParameters::voiceCountName = TRANS("Number of Voices");
const String ChorusParameters::voiceCountLabel = "";
const float ChorusParameters::voiceCountMin = 2.0f;
const float ChorusParameters::voiceCountMax = 5.0f;
const float ChorusParameters::voiceCountDefault = 3.0f;
const float ChorusParameters::voiceCountStep = 1.0f;
// LFO Frequency
const String ChorusParameters::lfoFreqID = "lfoFreq";
const String ChorusParameters::lfoFreqName = TRANS("LFO Freq");
const String ChorusParameters::lfoFreqLabel = "Hz";
const float ChorusParameters::lfoFreqMin = 0.05f;
const float ChorusParameters::lfoFreqMax = 2.0f;
const float ChorusParameters::lfoFreqDefault = 0.2f;
const float ChorusParameters::lfoFreqStep = 0.025f;
// Stereo
const String ChorusParameters::stereoID = "stereo";
const String ChorusParameters::stereoName = TRANS("Stereo");
const String ChorusParameters::stereoLabel = "";
const int ChorusParameters::stereoEnumCount = 2;
const int ChorusParameters::stereoDefault = 0;
// LFO Waveform
const String ChorusParameters::lfoWaveformID = "lfoWaveform";
const String ChorusParameters::lfoWaveformName = TRANS("LFO");
const String ChorusParameters::lfoWaveformLabel = "";
const int ChorusParameters::lfoWaveformEnumCount = 4;
const int ChorusParameters::lfoWaveformDefault = 0;
// Interpolation Type
const String ChorusParameters::interpTypeID = "interpType";
const String ChorusParameters::interpTypeName = TRANS("Interpolation Type");
const String ChorusParameters::interpTypeLabel = "";
const int ChorusParameters::interpTypeEnumCount = 3;
const int ChorusParameters::interpTypeDefault = 1;

AudioProcessorValueTreeState::ParameterLayout ChorusParameters::createParameterLayout()
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
        voiceCountID, voiceCountName,
        NormalisableRange<float>(voiceCountMin, voiceCountMax, voiceCountStep), voiceCountDefault,
        voiceCountLabel,
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

ChorusParameters::ChorusParameters(AudioProcessorValueTreeState& vts)
    : minDelayMs(minDelayDefault)
    , sweepWidthMs(sweepWidthDefault)
    , depth(depthDefault)
    , voiceCount(int(voiceCountDefault))
    , lfoFreqHz(lfoFreqDefault)
    , stereoMode(stereoDefault)
    , lfoWaveform((ChorusLFO::Waveform)lfoWaveformDefault)
    , interpolationType((ChorusInterpolation::Type)interpTypeDefault)
    , valueTreeState(vts)
    , minDelayListener(minDelayMs)
    , sweepWidthListener(sweepWidthMs)
    , depthListener(depth, 0.01f)
    , voiceCountListener(voiceCount)
    , lfoFreqListener(lfoFreqHz)
    , stereoListener(stereoMode)
    , lfoWaveformListener(lfoWaveform)
    , interpTypeListener(interpolationType)
{
    valueTreeState.addParameterListener(minDelayID, &minDelayListener);
    valueTreeState.addParameterListener(sweepWidthID, &sweepWidthListener);
    valueTreeState.addParameterListener(depthID, &depthListener);
    valueTreeState.addParameterListener(voiceCountID, &voiceCountListener);
    valueTreeState.addParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.addParameterListener(stereoID, &stereoListener);
    valueTreeState.addParameterListener(lfoWaveformID, &lfoWaveformListener);
    valueTreeState.addParameterListener(interpTypeID, &interpTypeListener);

    depth *= 0.01f;
}

ChorusParameters::~ChorusParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(minDelayID, &minDelayListener);
    valueTreeState.removeParameterListener(sweepWidthID, &sweepWidthListener);
    valueTreeState.removeParameterListener(depthID, &depthListener);
    valueTreeState.removeParameterListener(voiceCountID, &voiceCountListener);
    valueTreeState.removeParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.removeParameterListener(stereoID, &stereoListener);
    valueTreeState.removeParameterListener(lfoWaveformID, &lfoWaveformListener);
    valueTreeState.removeParameterListener(interpTypeID, &interpTypeListener);
}

void ChorusParameters::detachControls()
{
    minDelayAttachment.reset(nullptr);
    sweepWidthAttachment.reset(nullptr);
    depthAttachment.reset(nullptr);
    voiceCountAttachment.reset(nullptr);
    lfoFreqAttachment.reset(nullptr);
    stereoAttachment.reset(nullptr);
    lfoWaveformAttachment.reset(nullptr);
    interpTypeAttachment.reset(nullptr);
}

void ChorusParameters::attachControls(
    Slider& minDelayKnob,
    Slider& sweepWidthKnob,
    Slider& depthKnob,
    Slider& voiceCountKnob,
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
    voiceCountAttachment.reset(new SlAt(valueTreeState, voiceCountID, voiceCountKnob));
    lfoFreqAttachment.reset(new SlAt(valueTreeState, lfoFreqID, lfoFreqKnob));
    stereoAttachment.reset(new TbAt(valueTreeState, stereoID, stereoButton));
    lfoWaveformAttachment.reset(new CbAt(valueTreeState, lfoWaveformID, lfoWaveformCombo));
    interpTypeAttachment.reset(new CbAt(valueTreeState, interpTypeID, interpTypeCombo));
}