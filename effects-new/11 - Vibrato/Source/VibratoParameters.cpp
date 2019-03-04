#include "../JuceLibraryCode/JuceHeader.h"
#include "VibratoParameters.h"



const String VibratoParameters::interpolationID = "interpolationType";
const String VibratoParameters::interpolationName = TRANS("Interpolation Type");
const String VibratoParameters::interpolationLabel = "";
const String VibratoParameters::lfoWaveformID = "lfoWaveform";
const String VibratoParameters::lfoWaveformName = TRANS("LFO Waveform");
const String VibratoParameters::lfoWaveformLabel = "";
const String VibratoParameters::lfoFreqID = "lfoFreq";
const String VibratoParameters::lfoFreqName = TRANS("LFO Freq");
const String VibratoParameters::lfoFreqLabel = TRANS("Hz");
const String VibratoParameters::sweepWidthID = "sweepWidth";
const String VibratoParameters::sweepWidthName = TRANS("Sweep Width");
const String VibratoParameters::sweepWidthLabel = "%";



AudioProcessorValueTreeState::ParameterLayout VibratoParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<AudioParameterInt>(
                                                         interpolationID, interpolationName,
                                                         0, 3 - 1, 0,
                                                         interpolationLabel,
                                                         [](int value, int maxLength) { return String(value).substring(0, maxLength); },
                                                         [](const String& text) { return text.getIntValue(); }));
    
    params.push_back(std::make_unique<AudioParameterInt>(
                                                         lfoWaveformID, lfoWaveformName,
                                                         0, int(LFO_2::kNumWaveforms) - 1, int(LFO_2::kWaveformSine),
                                                         lfoWaveformLabel,
                                                         [](int value, int maxLength) { return String(value).substring(0, maxLength); },
                                                         [](const String& text) { return text.getIntValue(); }));
    
    params.push_back(std::make_unique<AudioParameterFloat>(
                                                           lfoFreqID, lfoFreqName,
                                                           NormalisableRange<float>(0.1f, 20.0f), 2.0f,
                                                           lfoFreqLabel,
                                                           AudioProcessorParameter::genericParameter,
                                                           [](float value, int maxLength) { return String(value).substring(0, maxLength); },
                                                           [](const String& text) { return text.getFloatValue(); }));
    
    params.push_back(std::make_unique<AudioParameterFloat>(
                                                           sweepWidthID, sweepWidthName,
                                                           NormalisableRange<float>(0.0f, 1.0f), 0.01f,
                                                           sweepWidthLabel,
                                                           AudioProcessorParameter::genericParameter,
                                                           [](float value, int maxLength) { return String(value).substring(0, maxLength); },
                                                           [](const String& text) { return text.getFloatValue(); }));
    
    
    return { params.begin(), params.end() };
}



VibratoParameters::VibratoParameters(AudioProcessorValueTreeState& vts)
    : lfoWaveform(LFO_2::kWaveformSine)
    , interpolation(Vibrato::kInterpolationNearestNeighbour)
    , lfoFreqHz(2.0f)
    , sweepWidth(0.01f)
    , valueTreeState(vts)
    , interpolationListener(interpolation)
    , lfoWaveformListener(lfoWaveform)
    , lfoFreqListener(lfoFreqHz)
    , sweepWidthListener(sweepWidth, 0.01f)


{
    
    valueTreeState.addParameterListener(lfoWaveformID, &lfoWaveformListener);
    valueTreeState.addParameterListener(interpolationID, &interpolationListener);
    valueTreeState.addParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.addParameterListener(sweepWidthID, &sweepWidthListener);


}

VibratoParameters::~VibratoParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(lfoWaveformID, &lfoWaveformListener);
    valueTreeState.removeParameterListener(interpolationID, &interpolationListener);
    valueTreeState.removeParameterListener(lfoFreqID, &lfoFreqListener);
    valueTreeState.removeParameterListener(sweepWidthID, &sweepWidthListener);
}


void VibratoParameters::detachControls()
{
    lfoWaveformAttachment.reset(nullptr);
    interpolationAttachment.reset(nullptr);
    lfoFreqAttachment.reset(nullptr);
    sweepWidthAttachment.reset(nullptr);
    
    
}


void VibratoParameters::attachControls(ComboBox& lfoWaveformCombo, ComboBox& interpolationCombo, Slider& lfoFreqKnob, Slider& sweepWidthKnob)
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    lfoWaveformAttachment.reset(new CbAt(valueTreeState, lfoWaveformID, lfoWaveformCombo));
    interpolationAttachment.reset(new CbAt(valueTreeState, interpolationID, interpolationCombo));
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    lfoFreqAttachment.reset(new SlAt(valueTreeState, lfoFreqID, lfoFreqKnob));
    sweepWidthAttachment.reset(new SlAt(valueTreeState, sweepWidthID, sweepWidthKnob));
}
