#include "GainParameters.h"

// Names are symbolic names used to tag AudioParameter objects and XML attributes
// Labels are human-friendly identifiers for use in GUIs
const String GainParameters::gainID = "gain";
const String GainParameters::gainName = TRANS("Gain");
const String GainParameters::gainLabel = TRANS("dB");

GainParameters::GainParameters(AudioProcessorValueTreeState& vts)
    : dbGain(0.0f), linearGain(1.0f)
    , valueTreeState(vts)
    , gainListener(linearGain, -100.0f)
{
}

GainParameters::~GainParameters()
{
    detachControls();
    valueTreeState.removeParameterListener(gainID, &gainListener);
}

void GainParameters::createAllParameters()
{
    // gain: float parameter, range -100.0 t0 +12.0 dB
    valueTreeState.createAndAddParameter(std::make_unique<AudioProcessorValueTreeState::Parameter>(
        gainID, gainName, gainLabel,
        NormalisableRange<float>(-100.0f, 12.0f),
        dbGain,
        [](float value) { return String(value); },
        [](const String& text) { return text.getFloatValue(); }) );
    valueTreeState.addParameterListener(gainID, &gainListener);
}

void GainParameters::detachControls()
{
    gainAttachment.reset(nullptr);
}

void GainParameters::attachControls(Slider& gainSlider)
{
    gainAttachment.reset(new SliderAttachment(valueTreeState, gainID, gainSlider));
}
