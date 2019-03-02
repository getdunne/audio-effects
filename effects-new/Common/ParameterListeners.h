#pragma once
#include "JuceHeader.h"

struct BoolListener : public AudioProcessorValueTreeState::Listener
{
    bool& workingValue;

    BoolListener(bool& wv) : AudioProcessorValueTreeState::Listener(), workingValue(wv) {}
    void parameterChanged(const String&, float newValue) override
    {
        workingValue = newValue >= 0.5f;
    }
};

struct IntegerListener : public AudioProcessorValueTreeState::Listener
{
    int& workingValue;

    IntegerListener(int& wv) : AudioProcessorValueTreeState::Listener(), workingValue(wv) {}
    void parameterChanged(const String&, float newValue) override
    {
        workingValue = (int)newValue;
    }
};

struct FloatListener : public AudioProcessorValueTreeState::Listener
{
    float& workingValue;
    float scaleFactor;      // multiply parameter values by this to get working value

    FloatListener(float& wv, float sf = 1.0f) : AudioProcessorValueTreeState::Listener(), workingValue(wv), scaleFactor(sf) {}
    void parameterChanged(const String&, float newValue) override
    {
        workingValue = scaleFactor * newValue;
    }
};

// This specialized AudioProcessorValueTreeState::Listener converts decibel gain to linear gain
struct FloatDecibelListener : public AudioProcessorValueTreeState::Listener
{
    float& workingValue;
    float minusInfinitydB;

    FloatDecibelListener(float& wv, float minusInfDB)
        : AudioProcessorValueTreeState::Listener()
        , workingValue(wv)
        , minusInfinitydB(minusInfDB)
    {}
    void parameterChanged(const String&, float newValue) override
    {
        workingValue = Decibels::decibelsToGain<float>(newValue, minusInfinitydB);
    }
};

// This specialized AudioProcessorValueTreeState::Listener converts a 0-based integer to LFO::Waveform.
// Because we have multiple LFO classes with different Waveform enums, we use a templatized declaration.
template <typename Waveform>
struct WaveformListener : public AudioProcessorValueTreeState::Listener
{
    Waveform& waveform;

    WaveformListener(Waveform& wf) : AudioProcessorValueTreeState::Listener(), waveform(wf) {}
    void parameterChanged(const String&, float newValue) override
    {
        waveform = (Waveform)(int(newValue + 0.5f));
    }
};
