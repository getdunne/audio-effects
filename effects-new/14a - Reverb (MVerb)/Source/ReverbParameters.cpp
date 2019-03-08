#include "ReverbParameters.h"

// Damping
const String ReverbParameters::dampingID = "damping";
const String ReverbParameters::dampingName = TRANS("Damping");
const String ReverbParameters::dampingLabel = "";
const float ReverbParameters::dampingMin = 0.0f;
const float ReverbParameters::dampingMax = 1.0f;
const float ReverbParameters::dampingDefault = 0.5f;
const float ReverbParameters::dampingStep = 0.01f;
// Density
const String ReverbParameters::densityID = "density";
const String ReverbParameters::densityName = TRANS("Density");
const String ReverbParameters::densityLabel = "";
const float ReverbParameters::densityMin = 0.0f;
const float ReverbParameters::densityMax = 1.0f;
const float ReverbParameters::densityDefault = 0.75f;
const float ReverbParameters::densityStep = 0.01f;
// Bandwidth
const String ReverbParameters::bandwidthID = "bandwidth";
const String ReverbParameters::bandwidthName = TRANS("Bandwidth");
const String ReverbParameters::bandwidthLabel = "";
const float ReverbParameters::bandwidthMin = 0.0f;
const float ReverbParameters::bandwidthMax = 1.0f;
const float ReverbParameters::bandwidthDefault = 1.0f;
const float ReverbParameters::bandwidthStep = 0.01f;
// Decay
const String ReverbParameters::decayID = "decay";
const String ReverbParameters::decayName = TRANS("Decay");
const String ReverbParameters::decayLabel = "";
const float ReverbParameters::decayMin = 0.0f;
const float ReverbParameters::decayMax = 1.0f;
const float ReverbParameters::decayDefault = 0.75f;
const float ReverbParameters::decayStep = 0.01f;
// Predelay
const String ReverbParameters::predelayID = "predelay";
const String ReverbParameters::predelayName = TRANS("Predelay");
const String ReverbParameters::predelayLabel = "";
const float ReverbParameters::predelayMin = 0.0f;
const float ReverbParameters::predelayMax = 1.0f;
const float ReverbParameters::predelayDefault = 0.0f;
const float ReverbParameters::predelayStep = 0.01f;
// Size
const String ReverbParameters::roomSizeID = "roomSize";
const String ReverbParameters::roomSizeName = TRANS("Room Size");
const String ReverbParameters::roomSizeLabel = "";
const float ReverbParameters::roomSizeMin = 0.0f;
const float ReverbParameters::roomSizeMax = 1.0f;
const float ReverbParameters::roomSizeDefault = 0.8f;
const float ReverbParameters::roomSizeStep = 0.01f;
// Mix
const String ReverbParameters::dryWetMixID = "dryWetMix";
const String ReverbParameters::dryWetMixName = TRANS("Mix");
const String ReverbParameters::dryWetMixLabel = "";
const float ReverbParameters::dryWetMixMin = 0.0f;
const float ReverbParameters::dryWetMixMax = 1.0f;
const float ReverbParameters::dryWetMixDefault = 0.5f;
const float ReverbParameters::dryWetMixStep = 0.01f;
// Early Reflection Mix
const String ReverbParameters::earlyLateMixID = "earlyLateMix";
const String ReverbParameters::earlyLateMixName = TRANS("Early/Late Mix");
const String ReverbParameters::earlyLateMixLabel = "";
const float ReverbParameters::earlyLateMixMin = 0.0f;
const float ReverbParameters::earlyLateMixMax = 1.0f;
const float ReverbParameters::earlyLateMixDefault = 0.5f;
const float ReverbParameters::earlyLateMixStep = 0.01f;

AudioProcessorValueTreeState::ParameterLayout ReverbParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        dampingID, dampingName,
        NormalisableRange<float>(dampingMin, dampingMax, dampingStep), dampingDefault,
        dampingLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        densityID, densityName,
        NormalisableRange<float>(densityMin, densityMax, densityStep), densityDefault,
        densityLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        bandwidthID, bandwidthName,
        NormalisableRange<float>(bandwidthMin, bandwidthMax, bandwidthStep), bandwidthDefault,
        bandwidthLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        decayID, decayName,
        NormalisableRange<float>(decayMin, decayMax, decayStep), decayDefault,
        decayLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        predelayID, predelayName,
        NormalisableRange<float>(predelayMin, predelayMax, predelayStep), predelayDefault,
        predelayLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        roomSizeID, roomSizeName,
        NormalisableRange<float>(roomSizeMin, roomSizeMax, roomSizeStep), roomSizeDefault,
        roomSizeLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        dryWetMixID, dryWetMixName,
        NormalisableRange<float>(dryWetMixMin, dryWetMixMax, dryWetMixStep), dryWetMixDefault,
        dryWetMixLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        earlyLateMixID, earlyLateMixName,
        NormalisableRange<float>(earlyLateMixMin, earlyLateMixMax, earlyLateMixStep), earlyLateMixDefault,
        earlyLateMixLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

ReverbParameters::ReverbParameters(AudioProcessorValueTreeState& vts,
    AudioProcessorValueTreeState::Listener* processor)
    : damping(dampingDefault)
    , density(densityDefault)
    , bandwidth(bandwidthDefault)
    , decay(decayDefault)
    , predelay(predelayDefault)
    , roomSize(roomSizeDefault)
    , dryWetMix(dryWetMixDefault)
    , earlyLateMix(earlyLateMixDefault)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , dampingListener(damping)
    , densityListener(density)
    , bandwidthListener(bandwidth)
    , decayListener(decay)
    , predelayListener(predelay)
    , roomSizeListener(roomSize)
    , dryWetMixListener(dryWetMix)
    , earlyLateMixListener(earlyLateMix)
{
    valueTreeState.addParameterListener(dampingID, processorAsListener);
    valueTreeState.addParameterListener(densityID, processorAsListener);
    valueTreeState.addParameterListener(bandwidthID, processorAsListener);
    valueTreeState.addParameterListener(decayID, processorAsListener);
    valueTreeState.addParameterListener(predelayID, processorAsListener);
    valueTreeState.addParameterListener(roomSizeID, processorAsListener);
    valueTreeState.addParameterListener(dryWetMixID, processorAsListener);
    valueTreeState.addParameterListener(earlyLateMixID, processorAsListener);

    valueTreeState.addParameterListener(dampingID, &dampingListener);
    valueTreeState.addParameterListener(densityID, &densityListener);
    valueTreeState.addParameterListener(bandwidthID, &bandwidthListener);
    valueTreeState.addParameterListener(decayID, &decayListener);
    valueTreeState.addParameterListener(predelayID, &predelayListener);
    valueTreeState.addParameterListener(roomSizeID, &roomSizeListener);
    valueTreeState.addParameterListener(dryWetMixID, &dryWetMixListener);
    valueTreeState.addParameterListener(earlyLateMixID, &earlyLateMixListener);
}

ReverbParameters::~ReverbParameters()
{
    detachControls();

    valueTreeState.removeParameterListener(dampingID, processorAsListener);
    valueTreeState.removeParameterListener(densityID, processorAsListener);
    valueTreeState.removeParameterListener(bandwidthID, processorAsListener);
    valueTreeState.removeParameterListener(decayID, processorAsListener);
    valueTreeState.removeParameterListener(predelayID, processorAsListener);
    valueTreeState.removeParameterListener(roomSizeID, processorAsListener);
    valueTreeState.removeParameterListener(dryWetMixID, processorAsListener);
    valueTreeState.removeParameterListener(earlyLateMixID, processorAsListener);

    valueTreeState.removeParameterListener(dampingID, &dampingListener);
    valueTreeState.removeParameterListener(densityID, &densityListener);
    valueTreeState.removeParameterListener(bandwidthID, &bandwidthListener);
    valueTreeState.removeParameterListener(decayID, &decayListener);
    valueTreeState.removeParameterListener(predelayID, &predelayListener);
    valueTreeState.removeParameterListener(roomSizeID, &roomSizeListener);
    valueTreeState.removeParameterListener(dryWetMixID, &dryWetMixListener);
    valueTreeState.removeParameterListener(earlyLateMixID, &earlyLateMixListener);
}

void ReverbParameters::detachControls()
{
    dampingAttachment.reset(nullptr);
    densityAttachment.reset(nullptr);
    bandwidthAttachment.reset(nullptr);
    decayAttachment.reset(nullptr);
    predelayAttachment.reset(nullptr);
    roomSizeAttachment.reset(nullptr);
    dryWetMixAttachment.reset(nullptr);
    earlyLateMixAttachment.reset(nullptr);
}

void ReverbParameters::attachControls(
    Slider& dampingKnob,
    Slider& densityKnob,
    Slider& bandwidthKnob,
    Slider& decayKnob,
    Slider& predelayKnob,
    Slider& roomSizeKnob,
    Slider& dryWetMixKnob,
    Slider& earlyLateMixKnob )
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    dampingAttachment.reset(new SlAt(valueTreeState, dampingID, dampingKnob));
    densityAttachment.reset(new SlAt(valueTreeState, densityID, densityKnob));
    bandwidthAttachment.reset(new SlAt(valueTreeState, bandwidthID, bandwidthKnob));
    decayAttachment.reset(new SlAt(valueTreeState, decayID, decayKnob));
    predelayAttachment.reset(new SlAt(valueTreeState, predelayID, predelayKnob));
    roomSizeAttachment.reset(new SlAt(valueTreeState, roomSizeID, roomSizeKnob));
    dryWetMixAttachment.reset(new SlAt(valueTreeState, dryWetMixID, dryWetMixKnob));
    earlyLateMixAttachment.reset(new SlAt(valueTreeState, earlyLateMixID, earlyLateMixKnob));
}