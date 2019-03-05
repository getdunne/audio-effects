#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ParameterListeners.h"
#include "LFO_2.h"
#include "Vibrato.h"


class VibratoParameters
{
    
    
public:
    // Id's are symbolic names, Names are human-friendly names for GUI
    // Labels are supplementary, typically used for units of measure
    static const String interpolationID, interpolationName, interpolationLabel;
    static const String lfoWaveformID, lfoWaveformName, lfoWaveformLabel;
    static const String lfoFreqID, lfoFreqName, lfoFreqLabel;
    static const String sweepWidthID, sweepWidthName, sweepWidthLabel;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    
public:
    VibratoParameters(AudioProcessorValueTreeState& vts);
    ~VibratoParameters();

    void detachControls();
    void attachControls(ComboBox& lfoWaveformCombo, ComboBox& interpolationCombo, Slider& lfoFreqKnob, Slider& sweepWidthKnob);
    
    // working parameter values
    LFO_2::Waveform lfoWaveform;
    Vibrato::Type interpolation;
    float lfoFreqHz;
    float sweepWidth;
    

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;
    
    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> lfoWaveformAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> interpolationAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sweepWidthAttachment;
    
    // Listener objects link parameters to working variables
    EnumListener<Vibrato::Type> interpolationListener;
    EnumListener<LFO_2::Waveform> lfoWaveformListener;
    FloatListener lfoFreqListener;
    FloatListener sweepWidthListener;
    
};
