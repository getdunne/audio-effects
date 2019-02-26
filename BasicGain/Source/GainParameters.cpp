#include "GainParameters.h"

// Names are symbolic names used to tag AudioParameter objects and XML attributes
// Labels are human-friendly identifiers for use in GUIs
const String GainParameters::gainID = "gain";
const String GainParameters::gainName = TRANS("Gain");
const String GainParameters::gainLabel = TRANS("x10");

GainParameters::GainParameters(AudioProcessorValueTreeState& vts)
    : valueTreeState(vts)
    , pGainAttachment(nullptr)
    , gainListener(gain, 0.1f)
{
    // Set default values of working values
    gain = 0.5f;
}

void GainParameters::createAllParameters()
{
    // gain: float parameter, range 0.0-1.0, shown as 0.0-10.0 (scaled x10)
    valueTreeState.createAndAddParameter(gainID, gainName, gainLabel,
        NormalisableRange<float>(0.0f, 10.0f),
        gain,
        [](float value) { return String(value); },
        [](const String& text) { return text.getFloatValue(); } );
    valueTreeState.addParameterListener(gainID, &gainListener);
}

void GainParameters::detachControls()
{
    if (pGainAttachment)
    {
        delete pGainAttachment;
        pGainAttachment = nullptr;
    }
}

void GainParameters::attachControls(Slider& gainSlider)
{
    detachControls();   // destroy existing attachments, if any

    pGainAttachment = new SliderAttachment(valueTreeState, gainID, gainSlider);
}

void GainParameters::putToXml(XmlElement* xml)
{
    // Set XML attributes based on working parameter values
    xml->setAttribute(gainName, gain);
}

void GainParameters::getFromXml(XmlElement* pXml)
{
    // Set parameters based on XML attributes
    // Parameter listeners will propagate to working values
    float fval = (float)pXml->getDoubleAttribute(gainName);
    valueTreeState.getParameterAsValue(gainID).setValue(fval);
}
