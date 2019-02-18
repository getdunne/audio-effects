/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Delay: basic delay effect with feedback
  See textbook Chapter 2: Delay Line Effects
 
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

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
*/
class DelayAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    DelayAudioProcessor();
    ~DelayAudioProcessor();

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
        kDelayLengthParam = 0,
        kDryMixParam,
        kWetMixParam,
        kFeedbackParam,
        kNumParameters
    };
    
    // Adjustable parameters:
    float delayLength_; // Length of delay line in seconds
    float dryMix_;      // Mix level of original signal (0-1)
    float wetMix_;      // Mix level of delayed signal (0-1)
    float feedback_;    // Feedback level (0-just less than 1)
    
private:
    // Circular buffer variables for implementing delay
    AudioSampleBuffer delayBuffer_;
    int delayBufferLength_;
    int delayReadPosition_, delayWritePosition_;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessor);
};

#endif  // __PLUGINPROCESSOR_H_4693CB6E__
