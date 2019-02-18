/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Ping-Pong Delay: stereo delay alternating between channels
  See textbook Chapter 2: Delay Line Effects
 
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

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PingPongDelayAudioProcessor::PingPongDelayAudioProcessor() : delayBuffer_ (2, 1)
{
    // Set default values:
    delayLengthLeft_ = delayLengthRight_ = 0.5;
    wetMix_ = 0.5;
    feedback_ = 0.75;
    delayBufferLength_ = 1;
    reverseChannels_ = false;
    
    // Start the circular buffer pointers at the beginning
    delayReadPositionLeft_ = delayReadPositionRight_ = 0;
    delayWritePosition_ = 0;
    
    lastUIWidth_ = 500;
    lastUIHeight_ = 140;
}

PingPongDelayAudioProcessor::~PingPongDelayAudioProcessor()
{
}

//==============================================================================
const String PingPongDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int PingPongDelayAudioProcessor::getNumParameters()
{
    return kNumParameters;
}

float PingPongDelayAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kWetMixParam:           return wetMix_;
        case kFeedbackParam:         return feedback_;
        case kDelayLengthLeftParam:  return delayLengthLeft_;
        case kDelayLengthRightParam: return delayLengthRight_;
        case kReverseChannelsParam:  return (reverseChannels_ ? 1.0f : 0.0f);
        default:                     return 0.0f;
    }
}

void PingPongDelayAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kWetMixParam:
            wetMix_ = newValue;
            break;
        case kFeedbackParam:
            feedback_ = newValue;
            break;
        case kDelayLengthLeftParam:
            delayLengthLeft_ = newValue;
            delayReadPositionLeft_ = (int)(delayWritePosition_ - (delayLengthLeft_ * getSampleRate())
                                       + delayBufferLength_) % delayBufferLength_;
            break;
        case kDelayLengthRightParam:
            delayLengthRight_ = newValue;
            delayReadPositionRight_ = (int)(delayWritePosition_ - (delayLengthRight_ * getSampleRate())
                                           + delayBufferLength_) % delayBufferLength_;
            break;
        case kReverseChannelsParam:
            reverseChannels_ = (newValue != 0.0f);
            break;
        default:
            break;
    }
}

const String PingPongDelayAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case kWetMixParam:           return "wet mix";
        case kFeedbackParam:         return "feedback";
        case kDelayLengthLeftParam:  return "delay left";
        case kDelayLengthRightParam: return "delay right";
        case kReverseChannelsParam:  return "reverse channels";
        default:                     break;
    }
    
    return String::empty;
}

const String PingPongDelayAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String PingPongDelayAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String PingPongDelayAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool PingPongDelayAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool PingPongDelayAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool PingPongDelayAudioProcessor::silenceInProducesSilenceOut() const
{
#if JucePlugin_SilenceInProducesSilenceOut
    return true;
#else
    return false;
#endif
}

double PingPongDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

bool PingPongDelayAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PingPongDelayAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int PingPongDelayAudioProcessor::getNumPrograms()
{
    return 0;
}

int PingPongDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PingPongDelayAudioProcessor::setCurrentProgram (int index)
{
}

const String PingPongDelayAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void PingPongDelayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void PingPongDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Allocate and zero the delay buffer (size will depend on current sample rate)
    // Sanity check the result so we don't end up with any zero-length calculations
    delayBufferLength_ = (int)(2.0*sampleRate);
    if(delayBufferLength_ < 1)
        delayBufferLength_ = 1;
    delayBuffer_.setSize(2, delayBufferLength_);
    delayBuffer_.clear();
    
    // This method gives us the sample rate. Use this to figure out what the delay position
    // offset should be (since it is specified in seconds, and we need to convert it to a number
    // of samples)
    delayReadPositionLeft_ = (int)(delayWritePosition_ - (delayLengthLeft_ * getSampleRate())
                               + delayBufferLength_) % delayBufferLength_;
    delayReadPositionRight_ = (int)(delayWritePosition_ - (delayLengthRight_ * getSampleRate())
                                   + delayBufferLength_) % delayBufferLength_;
}

void PingPongDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    
    // The delay buffer will stay in memory until the effect is unloaded.
}

void PingPongDelayAudioProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
    
    delayBuffer_.clear();
}


void PingPongDelayAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Helpful information about this block of samples:
    const int numInputChannels = getNumInputChannels();     // How many input channels for our effect?
    const int numOutputChannels = getNumOutputChannels();   // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();          // How many samples in the buffer for this block?

    // This shouldn't happen, but we need a sanity check: this effect only makes sense
    // if there are at least 2 channels to work with (and in this case only 2...)
    if(buffer.getNumChannels() < 2)
        return;
    
    // If there is one input only, the second channel may not contain anything useful.
    // start with a blank buffer in this case
    if(numInputChannels < 2)
        buffer.clear(1, 0, numSamples);
    
    // channelDataL and channelDataR are arrays of length numSamples which contain
    // the audio for one channel
    float *channelDataL = buffer.getWritePointer(0);
    float *channelDataR = buffer.getWritePointer(1);
        
    // delayDataL and delayDataR are the circular buffers for implementing delay
    float* delayDataL = delayBuffer_.getWritePointer(0);
    float* delayDataR = delayBuffer_.getWritePointer(1);
    
    for (int i = 0; i < numSamples; ++i)
    {
        const float inL = channelDataL[i];
        const float inR = channelDataR[i];
        float outL, outR;
        
        if(reverseChannels_)
        {
            outL = (inL + wetMix_ * delayDataR[delayReadPositionLeft_]);
            outR = (inR + wetMix_ * delayDataL[delayReadPositionRight_]);
        }
        else
        {
            outL = (inL + wetMix_ * delayDataL[delayReadPositionLeft_]);
            outR = (inR + wetMix_ * delayDataR[delayReadPositionRight_]);
        }
            
        // Store the output of one delay buffer into the other, producing
        // the ping-pong effect
        delayDataR[delayWritePosition_] = inR + (delayDataL[delayReadPositionLeft_] * feedback_);
        delayDataL[delayWritePosition_] = inL + (delayDataR[delayReadPositionRight_] * feedback_);
        
        if (++delayReadPositionLeft_ >= delayBufferLength_)
            delayReadPositionLeft_ = 0;
        if (++delayReadPositionRight_ >= delayBufferLength_)
            delayReadPositionRight_ = 0;
        if (++delayWritePosition_ >= delayBufferLength_)
            delayWritePosition_ = 0;
        
        // Store the output samples in the buffer, replacing the input
        channelDataL[i] = outL;
        channelDataR[i] = outR;
    }
    
    // Clear any channels above 2 (stereo)
    for (int i = 2; i < numOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool PingPongDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PingPongDelayAudioProcessor::createEditor()
{
    return new PingPongDelayAudioProcessorEditor (this);
}

//==============================================================================
void PingPongDelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // Create an outer XML element..
    XmlElement xml("C4DMPLUGINSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute("uiWidth", lastUIWidth_);
    xml.setAttribute("uiHeight", lastUIHeight_);
    xml.setAttribute("delayLengthLeft", delayLengthLeft_);
    xml.setAttribute("delayLengthRight", delayLengthRight_);
    xml.setAttribute("feedback", feedback_);
    xml.setAttribute("wetMix", wetMix_);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}

void PingPongDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if(xmlState != 0)
    {
        // make sure that it's actually our type of XML object..
        if(xmlState->hasTagName("C4DMPLUGINSETTINGS"))
        {
            // ok, now pull out our parameters..
            lastUIWidth_  = xmlState->getIntAttribute("uiWidth", lastUIWidth_);
            lastUIHeight_ = xmlState->getIntAttribute("uiHeight", lastUIHeight_);
            
            delayLengthLeft_  = (float)xmlState->getDoubleAttribute("delayLengthLeft", delayLengthLeft_);
            delayLengthRight_ = (float)xmlState->getDoubleAttribute("delayLengthRight", delayLengthRight_);
            feedback_     = (float)xmlState->getDoubleAttribute("feedback", feedback_);
            wetMix_       = (float)xmlState->getDoubleAttribute("wetMix", wetMix_);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PingPongDelayAudioProcessor();
}
