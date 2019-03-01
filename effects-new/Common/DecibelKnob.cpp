#include "DecibelKnob.h"

DecibelKnob::DecibelKnob(float minDB, float maxDB, float minusInfDB)
    : BasicKnob(minDB, maxDB)
    , minusInfinitydB(minusInfDB)
{
}

double DecibelKnob::getValueFromText(const String& text)
{
    auto decibelText = text.upToFirstOccurrenceOf("dB", false, false).trim();
    return decibelText.equalsIgnoreCase("-INF") ? minusInfinitydB
        : decibelText.getDoubleValue();
}

String DecibelKnob::getTextFromValue(double value)
{
    if (value == 0.0) return "0.0 dB";
    return Decibels::toString(value);
}
