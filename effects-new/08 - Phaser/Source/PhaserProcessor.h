#pragma once
#include "JuceHeader.h"
#include "PhaserParameters.h"
#include "OnePoleAllpassFilter.h"

class PhaserProcessor : public AudioProcessor
                      , public AudioProcessorValueTreeState::Listener
{
public:
    PhaserProcessor();
    ~PhaserProcessor();

    // How the plugin describes itself to the host
    const String getName() const override { return "Phaser"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    //bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    bool hasEditor() const override { return true; }
    AudioProcessorEditor* createEditor() override;

    // Multiple simultaneously-loaded presets aka "programs" (not used)
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const String getProgramName(int) override { return {}; }
    void changeProgramName(int, const String&) override {}

    // Actual audio processing
    void prepareToPlay (double sampleRate, int maxSamplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    // Enabling the host to save and restore state
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

public:
    // Plugin's AudioProcessorValueTreeState
    AudioProcessorValueTreeState valueTreeState;

    // Application's view of the AudioProcessorValueTreeState, including working parameter values
    PhaserParameters parameters;

    // Responding to parameter changes
    void parameterChanged(const String&, float) override;

protected:
    void allocateFilters(int numFilters);

private:
    float lfoPhase;             // Phase of the low-frequency oscillator
    double inverseSampleRate;   // Cache inverse of sample rate (more efficient to multiply than divide)
    unsigned int filterUpdateInterval;  // How often to update filter coefficients (samples)
    unsigned int sampleCount;   // counts samples up to filterUpdateInterval

    // Bank of allpass filters that do the phasing; N filters x M channels
    OwnedArray<OnePoleAllpassFilter> allpassFilters;

    // Storage of the last output sample from each bank of filters, for use in feedback loop
    float *lastFilterOutputs;
    int numLastFilterOutputs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaserProcessor)
};