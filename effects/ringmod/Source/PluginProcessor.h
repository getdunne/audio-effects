/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Ring Modulator: modulation using a carrier oscillator
  See textbook Chapter 5: Amplitude Modulation
 
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


//==============================================================================
/**
*/
class RingModulatorAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    RingModulatorAudioProcessor();
    ~RingModulatorAudioProcessor();

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
        kCarrierFrequencyParam,
        kSweepWidthParam,
        kLFOFrequencyParam,
        kWaveformParam,
        kNumParameters
    };
    
    enum Waveforms
    {
        kWaveformSine = 1,
        kWaveformTriangle,
        kWaveformSquare,
        kWaveformSawtooth,
        kWaveformInverseSawtooth,
        kNumWaveforms
    };
    
    // Adjustable parameters:
    float carrierFrequency_; // Frequency of main modulator (Hz)
    float sweepWidth_;    // Amount of change from min to max delay
    float lfoFrequency_;  // LFO frequency (Hz)
    int   waveform_;      // What shape should be used for the LFO
    
private:
    float lfo(float phase, int waveform);
    
    float lfoPhase_;     // Phase of the low-frequency oscillator
    float carrierPhase_; // Phase of the main (carrier) oscillator
    double inverseSampleRate_; // It's more efficient to multiply than divide, so
                               // cache the inverse of the sample rate
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RingModulatorAudioProcessor);
};

#endif  // __PLUGINPROCESSOR_H_4693CB6E__
