#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class ReverbParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String dampingID, dampingName, dampingLabel;
    static const float dampingMin, dampingMax, dampingDefault, dampingStep;
    static const String densityID, densityName, densityLabel;
    static const float densityMin, densityMax, densityDefault, densityStep;
    static const String bandwidthID, bandwidthName, bandwidthLabel;
    static const float bandwidthMin, bandwidthMax, bandwidthDefault, bandwidthStep;
    static const String decayID, decayName, decayLabel;
    static const float decayMin, decayMax, decayDefault, decayStep;
    static const String predelayID, predelayName, predelayLabel;
    static const float predelayMin, predelayMax, predelayDefault, predelayStep;
    static const String roomSizeID, roomSizeName, roomSizeLabel;
    static const float roomSizeMin, roomSizeMax, roomSizeDefault, roomSizeStep;
    static const String dryWetMixID, dryWetMixName, dryWetMixLabel;
    static const float dryWetMixMin, dryWetMixMax, dryWetMixDefault, dryWetMixStep;
    static const String earlyLateMixID, earlyLateMixName, earlyLateMixLabel;
    static const float earlyLateMixMin, earlyLateMixMax, earlyLateMixDefault, earlyLateMixStep;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    ReverbParameters(AudioProcessorValueTreeState& vts,
        AudioProcessorValueTreeState::Listener* processor);
    ~ReverbParameters();

    void detachControls();
    void attachControls(
        Slider& dampingKnob,
        Slider& densityKnob,
        Slider& bandwidthKnob,
        Slider& decayKnob,
        Slider& predelayKnob,
        Slider& roomSizeKnob,
        Slider& dryWetMixKnob,
        Slider& earlyLateMixKnob );

    // working parameter values
    float damping;
    float density;
    float bandwidth;
    float decay;
    float predelay;
    float roomSize;
    float dryWetMix;
    float earlyLateMix;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dampingAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> densityAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> bandwidthAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> predelayAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> roomSizeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dryWetMixAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> earlyLateMixAttachment;

    // Listener objects link parameters to working variables
    FloatListener dampingListener;
    FloatListener densityListener;
    FloatListener bandwidthListener;
    FloatListener decayListener;
    FloatListener predelayListener;
    FloatListener roomSizeListener;
    FloatListener dryWetMixListener;
    FloatListener earlyLateMixListener;
};