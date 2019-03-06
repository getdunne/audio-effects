#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"
#include "VibratoLFO.h"
#include "VibratoInterpolation.h"

class VibratoParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String lfoWaveformID, lfoWaveformName, lfoWaveformLabel;
    static const int lfoWaveformEnumCount, lfoWaveformDefault;
    static const String lfoFreqID, lfoFreqName, lfoFreqLabel;
    static const float lfoFreqMin, lfoFreqMax, lfoFreqDefault, lfoFreqStep;
    static const String sweepWidthID, sweepWidthName, sweepWidthLabel;
    static const float sweepWidthMin, sweepWidthMax, sweepWidthDefault, sweepWidthStep;
    static const String interpTypeID, interpTypeName, interpTypeLabel;
    static const int interpTypeEnumCount, interpTypeDefault;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    VibratoParameters(AudioProcessorValueTreeState& vts,
                      AudioProcessorValueTreeState::Listener* processor);
    ~VibratoParameters();

    void detachControls();
    void attachControls(
        ComboBox& lfoWaveformCombo,
        Slider& lfoFreqKnob,
        Slider& sweepWidthKnob,
        ComboBox& interpTypeCombo );

    // working parameter values
    VibratoLFO::Waveform lfoWaveform;
    float lfoFreqHz;
    float sweepWidthHz;
    VibratoInterpolation::Type interpolationType;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> lfoWaveformAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sweepWidthAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> interpTypeAttachment;

    // Listener objects link parameters to working variables
    EnumListener<VibratoLFO::Waveform> lfoWaveformListener;
    FloatListener lfoFreqListener;
    FloatListener sweepWidthListener;
    EnumListener<VibratoInterpolation::Type> interpTypeListener;
};