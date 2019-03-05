#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"
#include "RingModLFO.h"

class RingModParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI
    // Labels are supplementary, typically used for units of measure
    static const String carrierFreqID, carrierFreqName, carrierFreqLabel;
    static const String lfoWaveformID, lfoWaveformName, lfoWaveformLabel;
    static const String lfoFreqID, lfoFreqName, lfoFreqLabel;
    static const String lfoWidthID, lfoWidthName, lfoWidthLabel;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    RingModParameters(AudioProcessorValueTreeState& vts);
    ~RingModParameters();

    void detachControls();
    void attachControls(Slider& carrierFreqKnob,
                        ComboBox& lfoWaveformCombo, Slider& lfoFreqKnob, Slider& lfoWidthKnob);

    // working parameter values
    float carrierFreqHz;
    RingModLFO::Waveform lfoWaveform;
    float lfoFreqHz;
    float lfoWidthHz;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> carrierFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> lfoWaveformAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoWidthAttachment;

    // Listener objects link parameters to working variables
    FloatListener carrierFreqListener;
    EnumListener<RingModLFO::Waveform> lfoWaveformListener;
    FloatListener lfoFreqListener;
    FloatListener lfoWidthListener;
};
