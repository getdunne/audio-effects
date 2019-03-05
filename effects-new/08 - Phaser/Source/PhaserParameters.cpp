#include "PhaserParameters.h"

// Base Frequency
const String PhaserParameters::baseFreqID = "baseFreq";
const String PhaserParameters::baseFreqName = TRANS("Base Freq");
const String PhaserParameters::baseFreqLabel = "Hz";
const float PhaserParameters::baseFreqMin = 50.0f;
const float PhaserParameters::baseFreqMax = 1000.0f;
const float PhaserParameters::baseFreqDefault = 200.0f;
const float PhaserParameters::baseFreqStep = 0.5f;
// Sweep Width
const String PhaserParameters::sweepWidthID = "sweepWidth";
const String PhaserParameters::sweepWidthName = TRANS("Sweep Width");
const String PhaserParameters::sweepWidthLabel = "Hz";
const float PhaserParameters::sweepWidthMin = 50.0f;
const float PhaserParameters::sweepWidthMax = 5000.0f;
const float PhaserParameters::sweepWidthDefault = 2000.0f;
const float PhaserParameters::sweepWidthStep = 0.5f;
// Depth
const String PhaserParameters::depthID = "depth";
const String PhaserParameters::depthName = TRANS("");
const String PhaserParameters::depthLabel = "%";
const float PhaserParameters::depthMin = 0.0f;
const float PhaserParameters::depthMax = 100.0f;
const float PhaserParameters::depthDefault = 100.0f;
const float PhaserParameters::depthStep = 1.0f;
// Feedback
const String PhaserParameters::feedbackID = "feedback";
const String PhaserParameters::feedbackName = TRANS("Feedback");
const String PhaserParameters::feedbackLabel = "%";
const float PhaserParameters::feedbackMin = 0.0f;
const float PhaserParameters::feedbackMax = 99.0f;
const float PhaserParameters::feedbackDefault = 0.0f;
const float PhaserParameters::feedbackStep = 1.0f;
// LFO Frequency
const String PhaserParameters::lfoFreqID = "lfoFreq";
const String PhaserParameters::lfoFreqName = TRANS("LFO Freq");
const String PhaserParameters::lfoFreqLabel = "Hz";
const float PhaserParameters::lfoFreqMin = 0.05f;
const float PhaserParameters::lfoFreqMax = 2.0f;
const float PhaserParameters::lfoFreqDefault = 0.5f;
const float PhaserParameters::lfoFreqStep = 0.05f;
// Stereo
const String PhaserParameters::stereoID = "stereo";
const String PhaserParameters::stereoName = TRANS("Stereo");
const String PhaserParameters::stereoLabel = "";
const int PhaserParameters::stereoEnumCount = 2;
const int PhaserParameters::stereoDefault = 0;
// LFO Waveform
const String PhaserParameters::lfoWaveformID = "lfoWaveform";
const String PhaserParameters::lfoWaveformName = TRANS("LFO");
const String PhaserParameters::lfoWaveformLabel = "";
const int PhaserParameters::lfoWaveformEnumCount = 4;
const int PhaserParameters::lfoWaveformDefault = 0;
// Number of Filters
const String PhaserParameters::numFiltersID = "numFilters";
const String PhaserParameters::numFiltersName = TRANS("Number of Filters");
const String PhaserParameters::numFiltersLabel = "";
const int PhaserParameters::numFiltersEnumCount = 5;
const int PhaserParameters::numFiltersDefault = 1;

AudioProcessorValueTreeState::ParameterLayout PhaserParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        baseFreqID, baseFreqName,
        NormalisableRange<float>(baseFreqMin, baseFreqMax, baseFreqStep), baseFreqDefault,
        baseFreqLabel,
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
        numFiltersID, numFiltersName,
        0, numFiltersEnumCount - 1, numFiltersDefault,
        numFiltersLabel,
        [](int value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getIntValue(); }));

    return { params.begin(), params.end() };
}

PhaserParameters::PhaserParameters(AudioProcessorValueTreeState& vts,
                                   AudioProcessorValueTreeState::Listener* processor)
    : baseFreqHz(baseFreqDefault)
    , sweepWidthHz(sweepWidthDefault)
    , depth(depthDefault)
    , feedback(feedbackDefault)
    , lfoFreqHz(lfoFreqDefault)
    , stereoMode(stereoDefault)
    , lfoWaveform((PhaserLFO::Waveform)lfoWaveformDefault)
    , numFilters(numFiltersDefault)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , baseFreqListener(baseFreqHz)
    , sweepWidthListener(sweepWidthHz)
    , depthListener(depth, 0.01f)
    , feedbackListener(feedback, 0.01f)
    , lfoFreqListener(lfoFreqHz)
    , stereoListener(stereoMode)
    , lfoWaveformListener(lfoWaveform)
    , numFiltersListener(numFilters, 2, 2)
{
    valueTreeState.addParameterListener(baseFreqID, &baseFreqListener);
    valueTreeState.addParameterListener(sweepWidthID, &sweepWidthListener);
    valueTreeState.addParameterListener(depthID, &depthListener);
    valueTreeState.addParameterListener(feedbackID, &feedbackListener);
    valueTreeState.addParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.addParameterListener(stereoID, &stereoListener);
    valueTreeState.addParameterListener(lfoWaveformID, &lfoWaveformListener);
    valueTreeState.addParameterListener(numFiltersID, &numFiltersListener);
    valueTreeState.addParameterListener(numFiltersID, processorAsListener);

    depth = 1.0f;
    numFilters = 4;
}

PhaserParameters::~PhaserParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(baseFreqID, &baseFreqListener);
    valueTreeState.removeParameterListener(sweepWidthID, &sweepWidthListener);
    valueTreeState.removeParameterListener(depthID, &depthListener);
    valueTreeState.removeParameterListener(feedbackID, &feedbackListener);
    valueTreeState.removeParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.removeParameterListener(stereoID, &stereoListener);
    valueTreeState.removeParameterListener(lfoWaveformID, &lfoWaveformListener);
    valueTreeState.removeParameterListener(numFiltersID, &numFiltersListener);
    valueTreeState.removeParameterListener(numFiltersID, processorAsListener);
}

void PhaserParameters::detachControls()
{
    baseFreqAttachment.reset(nullptr);
    sweepWidthAttachment.reset(nullptr);
    depthAttachment.reset(nullptr);
    feedbackAttachment.reset(nullptr);
    lfoFreqAttachment.reset(nullptr);
    stereoAttachment.reset(nullptr);
    lfoWaveformAttachment.reset(nullptr);
    numFiltersAttachment.reset(nullptr);
}

void PhaserParameters::attachControls(
    Slider& baseFreqKnob,
    Slider& sweepWidthKnob,
    Slider& depthKnob,
    Slider& feedbackKnob,
    Slider& lfoFreqKnob,
    ToggleButton& stereoButton,
    ComboBox& lfoWaveformCombo,
    ComboBox& numFiltersCombo )
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using TbAt = AudioProcessorValueTreeState::ButtonAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    baseFreqAttachment.reset(new SlAt(valueTreeState, baseFreqID, baseFreqKnob));
    sweepWidthAttachment.reset(new SlAt(valueTreeState, sweepWidthID, sweepWidthKnob));
    depthAttachment.reset(new SlAt(valueTreeState, depthID, depthKnob));
    feedbackAttachment.reset(new SlAt(valueTreeState, feedbackID, feedbackKnob));
    lfoFreqAttachment.reset(new SlAt(valueTreeState, lfoFreqID, lfoFreqKnob));
    stereoAttachment.reset(new TbAt(valueTreeState, stereoID, stereoButton));
    lfoWaveformAttachment.reset(new CbAt(valueTreeState, lfoWaveformID, lfoWaveformCombo));
    numFiltersAttachment.reset(new CbAt(valueTreeState, numFiltersID, numFiltersCombo));
}