#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"
#include "PhaserLFO.h"

class PhaserParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String baseFreqID, baseFreqName, baseFreqLabel;
    static const float baseFreqMin, baseFreqMax, baseFreqDefault, baseFreqStep;
    static const String sweepWidthID, sweepWidthName, sweepWidthLabel;
    static const float sweepWidthMin, sweepWidthMax, sweepWidthDefault, sweepWidthStep;
    static const String depthID, depthName, depthLabel;
    static const float depthMin, depthMax, depthDefault, depthStep;
    static const String feedbackID, feedbackName, feedbackLabel;
    static const float feedbackMin, feedbackMax, feedbackDefault, feedbackStep;
    static const String lfoFreqID, lfoFreqName, lfoFreqLabel;
    static const float lfoFreqMin, lfoFreqMax, lfoFreqDefault, lfoFreqStep;
    static const String stereoID, stereoName, stereoLabel;
    static const int stereoEnumCount, stereoDefault;
    static const String lfoWaveformID, lfoWaveformName, lfoWaveformLabel;
    static const int lfoWaveformEnumCount, lfoWaveformDefault;
    static const String numFiltersID, numFiltersName, numFiltersLabel;
    static const int numFiltersEnumCount, numFiltersDefault;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    PhaserParameters(AudioProcessorValueTreeState& vts,
                     AudioProcessorValueTreeState::Listener* processor);
    ~PhaserParameters();

    void detachControls();
    void attachControls(
        Slider& baseFreqKnob,
        Slider& sweepWidthKnob,
        Slider& depthKnob,
        Slider& feedbackKnob,
        Slider& lfoFreqKnob,
        ToggleButton& stereoButton,
        ComboBox& lfoWaveformCombo,
        ComboBox& numFiltersCombo );

    // working parameter values
    float baseFreqHz;
    float sweepWidthHz;
    float depth;
    float feedback;
    float lfoFreqHz;
    bool stereoMode;
    PhaserLFO::Waveform lfoWaveform;
    int numFilters;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> baseFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sweepWidthAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> depthAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> stereoAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> lfoWaveformAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> numFiltersAttachment;

    // Listener objects link parameters to working variables
    FloatListener baseFreqListener;
    FloatListener sweepWidthListener;
    FloatListener depthListener;
    FloatListener feedbackListener;
    FloatListener lfoFreqListener;
    BoolListener stereoListener;
    EnumListener<PhaserLFO::Waveform> lfoWaveformListener;
    IntegerListener numFiltersListener;
};