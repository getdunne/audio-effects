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

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessor::DelayAudioProcessor() : delayBuffer_ (2, 1)
{
    // Set default values:
    delayLength_ = 0.5;
    dryMix_ = 1.0;
    wetMix_ = 0.5;
    feedback_ = 0.75;
    delayBufferLength_ = 1;
    
    // Start the circular buffer pointers at the beginning
    delayReadPosition_ = 0;
    delayWritePosition_ = 0;
    
    lastUIWidth_ = 370;
    lastUIHeight_ = 140;
}

DelayAudioProcessor::~DelayAudioProcessor()
{
}

//==============================================================================
const String DelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int DelayAudioProcessor::getNumParameters()
{
    return kNumParameters;
}

float DelayAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kDryMixParam:     return dryMix_;
        case kWetMixParam:     return wetMix_;
        case kFeedbackParam:   return feedback_;
        case kDelayLengthParam:return delayLength_;
        default:               return 0.0f;
    }
}

void DelayAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kDryMixParam:
            dryMix_ = newValue;
            break;
        case kWetMixParam:
            wetMix_ = newValue;
            break;
        case kFeedbackParam:
            feedback_ = newValue;
            break;
        case kDelayLengthParam:
            delayLength_ = newValue;
            delayReadPosition_ = (int)(delayWritePosition_ - (delayLength_ * getSampleRate())
                                       + delayBufferLength_) % delayBufferLength_;
            break;
        default:
            break;
    }
}

const String DelayAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case kDryMixParam:     return "dry mix";
        case kWetMixParam:     return "wet mix";
        case kFeedbackParam:   return "feedback";
        case kDelayLengthParam:return "delay";
        default:               break;
    }
    
    return String::empty;
}

const String DelayAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String DelayAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String DelayAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool DelayAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool DelayAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool DelayAudioProcessor::silenceInProducesSilenceOut() const
{
#if JucePlugin_SilenceInProducesSilenceOut
    return true;
#else
    return false;
#endif
}
double DelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}
bool DelayAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool DelayAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int DelayAudioProcessor::getNumPrograms()
{
    return 0;
}

int DelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayAudioProcessor::setCurrentProgram (int index)
{
}

const String DelayAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void DelayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
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
    delayReadPosition_ = (int)(delayWritePosition_ - (delayLength_ * getSampleRate())
                               + delayBufferLength_) % delayBufferLength_;
}

void DelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    
    // The delay buffer will stay in memory until the effect is unloaded.
}

void DelayAudioProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
    
    delayBuffer_.clear();
}


void DelayAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Helpful information about this block of samples:
    const int numInputChannels = getNumInputChannels();     // How many input channels for our effect?
    const int numOutputChannels = getNumOutputChannels();   // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();          // How many samples in the buffer for this block?
    
    int channel, dpr, dpw; // dpr = delay read pointer; dpw = delay write pointer
    
    // Go through each channel of audio that's passed in. In this example we apply identical
    // effects to each channel, regardless of how many input channels there are. For some effects, like
    // a stereo chorus or panner, you might do something different for each channel.
    
    for (channel = 0; channel < numInputChannels; ++channel)
    {
        // channelData is an array of length numSamples which contains the audio for one channel
        float* channelData = buffer.getWritePointer(channel);
        
        // delayData is the circular buffer for implementing delay on this channel
        float* delayData = delayBuffer_.getWritePointer (jmin (channel, delayBuffer_.getNumChannels() - 1));
        
        // Make a temporary copy of any state variables declared in PluginProcessor.h which need to be
        // maintained between calls to processBlock(). Each channel needs to be processed identically
        // which means that the activity of processing one channel can't affect the state variable for
        // the next channel.
        
        dpr = delayReadPosition_;
        dpw = delayWritePosition_;
        
        for (int i = 0; i < numSamples; ++i)
        {
            const float in = channelData[i];
            float out = 0.0;
            
            // In this example, the output is the input plus the contents of the delay buffer (weighted by delayMix)
            // The last term implements a tremolo (variable amplitude) on the whole thing.
            
            out = (dryMix_ * in + wetMix_ * delayData[dpr]);
            
            // Store the current information in the delay buffer. delayData[dpr] is the delay sample we just read,
            // i.e. what came out of the buffer. delayData[dpw] is what we write to the buffer, i.e. what goes in
            
            delayData[dpw] = in + (delayData[dpr] * feedback_);
            
            if (++dpr >= delayBufferLength_)
                dpr = 0;
            if (++dpw >= delayBufferLength_)
                dpw = 0;
            
            // Store the output sample in the buffer, replacing the input
            channelData[i] = out;
        }
    }
    
    // Having made a local copy of the state variables for each channel, now transfer the result
    // back to the main state variable so they will be preserved for the next call of processBlock()
    
    delayReadPosition_ = dpr;
    delayWritePosition_ = dpw;
    
    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = numInputChannels; i < numOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool DelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
    return new DelayAudioProcessorEditor (this);
}

//==============================================================================
void DelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // Create an outer XML element..
    XmlElement xml("C4DMPLUGINSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute("uiWidth", lastUIWidth_);
    xml.setAttribute("uiHeight", lastUIHeight_);
    xml.setAttribute("delayLength", delayLength_);
    xml.setAttribute("feedback", feedback_);
    xml.setAttribute("dryMix", dryMix_);
    xml.setAttribute("wetMix", wetMix_);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}

void DelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
            
            delayLength_  = (float)xmlState->getDoubleAttribute("delayLength", delayLength_);
            feedback_     = (float)xmlState->getDoubleAttribute("feedback", feedback_);
            dryMix_       = (float)xmlState->getDoubleAttribute("dryMix", dryMix_);
            wetMix_       = (float)xmlState->getDoubleAttribute("wetMix", wetMix_);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}
