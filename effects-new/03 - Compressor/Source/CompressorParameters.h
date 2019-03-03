#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class CompressorParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI
    // Labels are supplementary, typically used for units of measure
    static const String ratioID, ratioName, ratioLabel;
    static const String attackID, attackName, attackLabel;
    static const String releaseID, releaseName, releaseLabel;
    static const String thresholdID, thresholdName, thresholdLabel;
    static const String gainID, gainName, gainLabel;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    CompressorParameters(AudioProcessorValueTreeState& vts);
    ~CompressorParameters();

    void detachControls();
    void attachControls(Slider& ratioKnob, Slider& attackKnob, Slider& releaseKnob,
                        Slider& thresholdKnob, Slider& gainKnob);

    // working parameter values
    float compressionRatio;
    float attackTimeMs, releaseTimeMs;
    float threshold;
    float makeUpGain;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> ratioAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    // Listener objects link parameters to working variables
    FloatListener ratioListener;
    FloatListener attackListener;
    FloatListener releaseListener;
    FloatListener thresholdListener;
    FloatListener gainListener;
};
