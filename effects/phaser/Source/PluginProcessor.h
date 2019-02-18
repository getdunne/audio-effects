/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Phaser: phasing effect using time-varying allpass filters
  See textbook Chapter 4: Filter Effects
 
  Code by Andrew McPherson, Brecht De Man and Joshua Reiss
 
  ---

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __PLUGINPROCESSOR_H_4693CB6E__
#define __PLUGINPROCESSOR_H_4693CB6E__

#define _USE_MATH_DEFINES
#include "../JuceLibraryCode/JuceHeader.h"
#include "OnePoleAllpassFilter.h"
#include "TwoPoleAllpassFilter.h"

//==============================================================================
/**
*/
class PhaserAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    PhaserAudioProcessor();
    ~PhaserAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();
    void reset();
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
    

    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;

    //==============================================================================
    const String getName() const;

    int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool silenceInProducesSilenceOut() const;
    double getTailLengthSeconds() const;    
    bool acceptsMidi() const;
    bool producesMidi() const;

    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);
    
    //==============================================================================
    
    // these are used to persist the UI's size - the values are stored along with the
    // filter's other parameters, and the UI component will update them when it gets
    // resized.
    int lastUIWidth_, lastUIHeight_;
    
    enum Parameters
    {
        kBaseFrequencyParam = 0,
        kSweepWidthParam,
        kDepthParam,
        kFeedbackParam,
        kLFOFrequencyParam,
        kFiltersParam,
        kWaveformParam,
        kStereoParam,
        kNumParameters
    };
    
    enum Waveforms
    {
        kWaveformSine = 1,
        kWaveformTriangle,
        kWaveformSquare,
        kWaveformSawtooth,
        kNumWaveforms
    };
    
    // Adjustable parameters:
    float baseFrequency_; // Lowest frequency of allpass filters
    float sweepWidth_;    // Amount of change from min to max delay
    float depth_;         // Mix level for phase-shifted signal (0-1)
    float feedback_;      // Feedback level for feedback phaser (0-<1)
    float lfoFrequency_;  // LFO frequency (Hz)
    int   filtersPerChannel_; // How many allpass filters to use
    int   waveform_;      // What shape should be used for the LFO
    int   stereo_;        // Whether to use stereo phasing
    
private:
    float lfo(float phase, int waveform);
    void allocateFilters();   // Create the filter objects...
    void deallocateFilters(); // Delete them...
    void reallocateFilters(); // Delete and rebuild in one combined operation
    
    CriticalSection lock_; // Lock for updating number of filters
    
    float lfoPhase_;   // Phase of the low-frequency oscillator
    double inverseSampleRate_; // It's more efficient to multiply than divide, so
                               // cache the inverse of the sample rate
    unsigned int sampleCount_; // Counter to keep track of samples elapsed, to
                               // keep track of filter updates
    unsigned int filterUpdateInterval_; // How often to update filter coefficients
    
    // Bank of allpass filters that do the phasing; N filters x M channels
    OnePoleAllpassFilter **allpassFilters_;
    
    // Storage of the last output sample from each bank of filters, for use in
    // feedback loop
    float *lastFilterOutputs_;
    int numLastFilterOutputs_;

    int totalNumFilters_;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaserAudioProcessor);
};

#endif  // __PLUGINPROCESSOR_H_4693CB6E__
