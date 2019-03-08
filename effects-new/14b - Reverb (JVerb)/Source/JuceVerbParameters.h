#pragma once
#include "JuceHeader.h"
#include "ParameterListeners.h"

class JuceVerbParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String roomSizeID, roomSizeName, roomSizeLabel;
    static const float roomSizeMin, roomSizeMax, roomSizeDefault, roomSizeStep;
    static const String dampingID, dampingName, dampingLabel;
    static const float dampingMin, dampingMax, dampingDefault, dampingStep;
    static const String wetLevelID, wetLevelName, wetLevelLabel;
    static const float wetLevelMin, wetLevelMax, wetLevelDefault, wetLevelStep;
    static const String dryLevelID, dryLevelName, dryLevelLabel;
    static const float dryLevelMin, dryLevelMax, dryLevelDefault, dryLevelStep;
    static const String widthID, widthName, widthLabel;
    static const float widthMin, widthMax, widthDefault, widthStep;
    static const String freezeModeID, freezeModeName, freezeModeLabel;
    static const int freezeModeEnumCount, freezeModeDefault;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    JuceVerbParameters(AudioProcessorValueTreeState& vts,
                       AudioProcessorValueTreeState::Listener* processor);
    ~JuceVerbParameters();

    void detachControls();
    void attachControls(
        Slider& roomSizeKnob,
        Slider& dampingKnob,
        Slider& wetLevelKnob,
        Slider& dryLevelKnob,
        Slider& widthKnob,
        ToggleButton& freezeModeToggle );

    // working parameter values
    float roomSize;
    float damping;
    float wetLevel;
    float dryLevel;
    float width;
    bool freezeMode;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> roomSizeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dampingAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> wetLevelAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dryLevelAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> freezeModeAttachment;

    // Listener objects link parameters to working variables
    FloatListener roomSizeListener;
    FloatListener dampingListener;
    FloatListener wetLevelListener;
    FloatListener dryLevelListener;
    FloatListener widthListener;
    BoolListener freezeModeListener;
};