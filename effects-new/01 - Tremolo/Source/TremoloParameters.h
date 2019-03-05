#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"
#include "TremoloLFO.h"

class TremoloParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI
    // Labels are supplementary, typically used for units of measure
    static const String lfoWaveformID, lfoWaveformName, lfoWaveformLabel;
    static const String lfoFreqID, lfoFreqName, lfoFreqLabel;
    static const String modDepthID, modDepthName, modDepthLabel;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    TremoloParameters(AudioProcessorValueTreeState& vts);
    ~TremoloParameters();

    void detachControls();
    void attachControls(ComboBox& lfoWaveformCombo, Slider& lfoFreqKnob, Slider& modDepthKnob);

    // working parameter values
    TremoloLFO::Waveform lfoWaveform;
    float lfoFreqHz;
    float modDepth;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> lfoWaveformAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> modDepthAttachment;

    // Listener objects link parameters to working variables
    EnumListener<TremoloLFO::Waveform> lfoWaveformListener;
    FloatListener lfoFreqListener;
    FloatListener modDepthListener;
};
