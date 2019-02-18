/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Empty: template for an effect; passes input to output unmodified
  See textbook Chapter 13: Building Audio Effect Plug-Ins
 
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
EmptyAudioProcessor::EmptyAudioProcessor()
{
    // Set default values:
    
    volume_ = 1.0;
    lastUIWidth_ = 170;
    lastUIHeight_ = 100;
}

EmptyAudioProcessor::~EmptyAudioProcessor()
{
}

//==============================================================================
const String EmptyAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int EmptyAudioProcessor::getNumParameters()
{
    return kNumParameters;
}

float EmptyAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kVolumeParam:     return volume_;
        default:               return 0.0f;
    }
}

void EmptyAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kVolumeParam:
            volume_ = newValue;
            break;
        default:
            break;
    }
}

const String EmptyAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case kVolumeParam:     return "volume";
        default:               break;
    }
    
    return String::empty;
}

const String EmptyAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String EmptyAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String EmptyAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool EmptyAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool EmptyAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool EmptyAudioProcessor::silenceInProducesSilenceOut() const
{
#if JucePlugin_SilenceInProducesSilenceOut
    return true;
#else
    return false;
#endif
}

double EmptyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

bool EmptyAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool EmptyAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int EmptyAudioProcessor::getNumPrograms()
{
    return 0;
}

int EmptyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EmptyAudioProcessor::setCurrentProgram (int index)
{
}

const String EmptyAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void EmptyAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void EmptyAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void EmptyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void EmptyAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Helpful information about this block of samples:
    const int numInputChannels = getNumInputChannels();     // How many input channels for our effect?
    const int numOutputChannels = getNumOutputChannels();   // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();          // How many samples in the buffer for this block?
    
    // Go through each channel of audio that's passed in
    
    for (int channel = 0; channel < numInputChannels; ++channel)
    {
        // channelData is an array of length numSamples which contains the audio for one channel
        float* channelData = buffer.getWritePointer(channel);
        
        for (int i = 0; i < numSamples; ++i)
        {
            const float in = channelData[i];

            // Store the output sample back in the buffer
            channelData[i] = in * volume_;
        }
    }
    
    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = numInputChannels; i < numOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool EmptyAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* EmptyAudioProcessor::createEditor()
{
    return new EmptyAudioProcessorEditor (this);
}

//==============================================================================
void EmptyAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // Create an outer XML element..
    XmlElement xml("C4DMPLUGINSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute("uiWidth", lastUIWidth_);
    xml.setAttribute("uiHeight", lastUIHeight_);
    xml.setAttribute("volume", volume_);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}

void EmptyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
            
            volume_  = (float)xmlState->getDoubleAttribute("volume", volume_);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EmptyAudioProcessor();
}
