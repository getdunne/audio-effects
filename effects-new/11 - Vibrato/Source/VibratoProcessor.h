#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "VibratoParameters.h"

class VibratoProcessor  : public AudioProcessor
{
public:

    VibratoProcessor();
    ~VibratoProcessor();

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
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
    
    // Enabling the host to save and restore state
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    

    static const float kMaximumSweepWidth;
    
//    // Adjustable parameters:
//    float sweepWidth_; // Amount of change from min to max delay
//    float frequency_;  // LFO frequency (Hz)
//    int   waveform_;   // What shape should be used for the LFO
//    int   interpolation_; // What type of interpolation to use
//

    
public:
    // Plugin's AudioProcessorValueTreeState
    AudioProcessorValueTreeState valueTreeState;
    
    // Application's view of the AudioProcessorValueTreeState, including working parameter values
    VibratoParameters parameters;
    
    
private:
    

    
//    float lfo(float phase, int waveform);
//
    // Circular buffer variables for implementing delay
    AudioSampleBuffer delayBuffer_;
    int delayBufferLength_;
    int delayWritePosition_;
    
    float lfoPhase_;   // Phase of the low-frequency oscillator
    double inverseSampleRate_; // It's more efficient to multiply than divide, so
    // cache the inverse of the sample rate
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VibratoProcessor)
};
