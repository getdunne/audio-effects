#pragma once
#include "JuceHeader.h"
#include "CompressorParameters.h"

class CompressorProcessor : public AudioProcessor, public ChangeBroadcaster
{
public:
    CompressorProcessor();
    ~CompressorProcessor();

    // How the plugin describes itself to the host
    const String getName() const override { return JucePlugin_Name; }
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

    // Computed linear gain (for GUI display)
    float currentGain;

protected:
    void computeCompressionGain(AudioSampleBuffer &buffer, int m);

public:
    // Plugin's AudioProcessorValueTreeState, and its associated UndoManager
    AudioProcessorValueTreeState valueTreeState;

    // Application's view of the AudioProcessorValueTreeState, including working parameter values
    CompressorParameters parameters;

    int bufferSize;
    bool autoTime;
    AudioSampleBuffer inputBuffer;
    HeapBlock <float> x_g, x_l, y_g, y_l, c; // input, output, control
    float yL_prev;
    int samplerate;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorProcessor)
};
