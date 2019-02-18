/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Auto-Wah: LFO or envelope-operated wah effect
  See textbook Chapter 4: Filter Effects
 
  Code by Andrew McPherson, Brecht de Man and Joshua Reiss
 
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
#include "ResonantLowpassFilter.h"

//==============================================================================
/**
*/
class AutoWahAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    AutoWahAudioProcessor();
    ~AutoWahAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

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
        kBaseFrequencyParam = 0, /* Centre frequency in Hz */
        kQParam, /* Q of the resonant filter */
        kLFOFrequencyParam,
        kLFOWidthParam,
        kEnvelopeWidthParam,
        kEnvelopeAttackParam,
        kEnvelopeDecayParam,
        kNumParameters
    };
    
    float baseFrequency_, q_;
    float lfoFrequency_, lfoWidth_;
    float envelopeWidth_, envelopeAttack_, envelopeDecay_;
private:
    // Methods for creating and releasing memory associated with filters
    void allocateFilters();
    void deallocateFilters();
    
    float lfoPhase_;   // Phase of the low-frequency oscillator

    double *envelopes_; // Values of signal envelopes for each channel
    int numEnvelopes_;
    
    // Convert the attack and decay time constants to a multiplier for
    // a first-order lowpass filter
    double attackMultiplier_, decayMultiplier_;
    
    ResonantLowpassFilter **wahFilters_;
    int numWahFilters_;
    double inverseSampleRate_; // Save the inverse of the sample rate for faster calculation
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoWahAudioProcessor);
};

#endif  // __PLUGINPROCESSOR_H_4693CB6E__
