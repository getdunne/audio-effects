#include "JuceVerbParameters.h"

// Room Size
const String JuceVerbParameters::roomSizeID = "roomSize";
const String JuceVerbParameters::roomSizeName = TRANS("Room Size");
const String JuceVerbParameters::roomSizeLabel = "";
const float JuceVerbParameters::roomSizeMin = 0.0f;
const float JuceVerbParameters::roomSizeMax = 1.0f;
const float JuceVerbParameters::roomSizeDefault = 0.5f;
const float JuceVerbParameters::roomSizeStep = 0.0f;
// Damping
const String JuceVerbParameters::dampingID = "damping";
const String JuceVerbParameters::dampingName = TRANS("Damping");
const String JuceVerbParameters::dampingLabel = "";
const float JuceVerbParameters::dampingMin = 0.0f;
const float JuceVerbParameters::dampingMax = 1.0f;
const float JuceVerbParameters::dampingDefault = 0.5f;
const float JuceVerbParameters::dampingStep = 0.0f;
// Wet Level
const String JuceVerbParameters::wetLevelID = "wetLevel";
const String JuceVerbParameters::wetLevelName = TRANS("Wet Level");
const String JuceVerbParameters::wetLevelLabel = "";
const float JuceVerbParameters::wetLevelMin = 0.0f;
const float JuceVerbParameters::wetLevelMax = 1.0f;
const float JuceVerbParameters::wetLevelDefault = 0.33f;
const float JuceVerbParameters::wetLevelStep = 0.0f;
// Dry Level
const String JuceVerbParameters::dryLevelID = "dryLevel";
const String JuceVerbParameters::dryLevelName = TRANS("Dry Level");
const String JuceVerbParameters::dryLevelLabel = "";
const float JuceVerbParameters::dryLevelMin = 0.0f;
const float JuceVerbParameters::dryLevelMax = 1.0f;
const float JuceVerbParameters::dryLevelDefault = 0.4f;
const float JuceVerbParameters::dryLevelStep = 0.0f;
// Width
const String JuceVerbParameters::widthID = "width";
const String JuceVerbParameters::widthName = TRANS("Width");
const String JuceVerbParameters::widthLabel = "";
const float JuceVerbParameters::widthMin = 0.0f;
const float JuceVerbParameters::widthMax = 1.0f;
const float JuceVerbParameters::widthDefault = 1.0f;
const float JuceVerbParameters::widthStep = 0.0f;
// Freeze Mode
const String JuceVerbParameters::freezeModeID = "freezeMode";
const String JuceVerbParameters::freezeModeName = TRANS("Freeze");
const String JuceVerbParameters::freezeModeLabel = "";
const int JuceVerbParameters::freezeModeEnumCount = 2;
const int JuceVerbParameters::freezeModeDefault = 0;

AudioProcessorValueTreeState::ParameterLayout JuceVerbParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        roomSizeID, roomSizeName,
        NormalisableRange<float>(roomSizeMin, roomSizeMax, roomSizeStep), roomSizeDefault,
        roomSizeLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        dampingID, dampingName,
        NormalisableRange<float>(dampingMin, dampingMax, dampingStep), dampingDefault,
        dampingLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        wetLevelID, wetLevelName,
        NormalisableRange<float>(wetLevelMin, wetLevelMax, wetLevelStep), wetLevelDefault,
        wetLevelLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        dryLevelID, dryLevelName,
        NormalisableRange<float>(dryLevelMin, dryLevelMax, dryLevelStep), dryLevelDefault,
        dryLevelLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        widthID, widthName,
        NormalisableRange<float>(widthMin, widthMax, widthStep), widthDefault,
        widthLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterInt>(
        freezeModeID, freezeModeName,
        0, freezeModeEnumCount - 1, freezeModeDefault,
        freezeModeLabel,
        [](int value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getIntValue(); }));

    return { params.begin(), params.end() };
}

JuceVerbParameters::JuceVerbParameters(AudioProcessorValueTreeState& vts,
                                       AudioProcessorValueTreeState::Listener* processor)
    : roomSize(roomSizeDefault)
    , damping(dampingDefault)
    , wetLevel(wetLevelDefault)
    , dryLevel(dryLevelDefault)
    , width(widthDefault)
    , freezeMode(false)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , roomSizeListener(roomSize)
    , dampingListener(damping)
    , wetLevelListener(wetLevel)
    , dryLevelListener(dryLevel)
    , widthListener(width)
    , freezeModeListener(freezeMode)
{
    valueTreeState.addParameterListener(roomSizeID, processorAsListener);
    valueTreeState.addParameterListener(dampingID, processorAsListener);
    valueTreeState.addParameterListener(wetLevelID, processorAsListener);
    valueTreeState.addParameterListener(dryLevelID, processorAsListener);
    valueTreeState.addParameterListener(widthID, processorAsListener);
    valueTreeState.addParameterListener(freezeModeID, processorAsListener);

    valueTreeState.addParameterListener(roomSizeID, &roomSizeListener);
    valueTreeState.addParameterListener(dampingID, &dampingListener);
    valueTreeState.addParameterListener(wetLevelID, &wetLevelListener);
    valueTreeState.addParameterListener(dryLevelID, &dryLevelListener);
    valueTreeState.addParameterListener(widthID, &widthListener);
    valueTreeState.addParameterListener(freezeModeID, &freezeModeListener);
}

JuceVerbParameters::~JuceVerbParameters()
{
    detachControls();

    valueTreeState.removeParameterListener(roomSizeID, processorAsListener);
    valueTreeState.removeParameterListener(dampingID, processorAsListener);
    valueTreeState.removeParameterListener(wetLevelID, processorAsListener);
    valueTreeState.removeParameterListener(dryLevelID, processorAsListener);
    valueTreeState.removeParameterListener(widthID, processorAsListener);
    valueTreeState.removeParameterListener(freezeModeID, processorAsListener);

    valueTreeState.removeParameterListener(roomSizeID, &roomSizeListener);
    valueTreeState.removeParameterListener(dampingID, &dampingListener);
    valueTreeState.removeParameterListener(wetLevelID, &wetLevelListener);
    valueTreeState.removeParameterListener(dryLevelID, &dryLevelListener);
    valueTreeState.removeParameterListener(widthID, &widthListener);
    valueTreeState.removeParameterListener(freezeModeID, &freezeModeListener);
}

void JuceVerbParameters::detachControls()
{
    roomSizeAttachment.reset(nullptr);
    dampingAttachment.reset(nullptr);
    wetLevelAttachment.reset(nullptr);
    dryLevelAttachment.reset(nullptr);
    widthAttachment.reset(nullptr);
    freezeModeAttachment.reset(nullptr);
}

void JuceVerbParameters::attachControls(
    Slider& roomSizeKnob,
    Slider& dampingKnob,
    Slider& wetLevelKnob,
    Slider& dryLevelKnob,
    Slider& widthKnob,
    ToggleButton& freezeModeToggle)
{
    using TbAt = AudioProcessorValueTreeState::ButtonAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    roomSizeAttachment.reset(new SlAt(valueTreeState, roomSizeID, roomSizeKnob));
    dampingAttachment.reset(new SlAt(valueTreeState, dampingID, dampingKnob));
    wetLevelAttachment.reset(new SlAt(valueTreeState, wetLevelID, wetLevelKnob));
    dryLevelAttachment.reset(new SlAt(valueTreeState, dryLevelID, dryLevelKnob));
    widthAttachment.reset(new SlAt(valueTreeState, widthID, widthKnob));
    freezeModeAttachment.reset(new TbAt(valueTreeState, freezeModeID, freezeModeToggle));
}