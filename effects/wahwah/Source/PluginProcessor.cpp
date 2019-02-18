/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Wah-Wah: filter effect using variable-frequency filter
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

#include "PluginProcessor.h"
#include "PluginEditor.h"

// The filter will produce a resonant peak of amplitude Q; bring everything
// down somewhat to compensate, though try to maintain some perceptual balance
// of being similar loudness. (This factor has been chosen somewhat arbitrarily.)
const double kWahwahFilterGain = 0.5;

//==============================================================================
WahwahAudioProcessor::WahwahAudioProcessor()
{
    // Set default values:
    centreFrequency_ = 400.0;
    q_ = 5.0;
    
    // Initialise the filters later when we know how many channels
    wahFilters_ = 0;
    numWahFilters_ = 0;
    
    inverseSampleRate_ = 1.0/44100.0; // start with a sensible default
    
    lastUIWidth_ = 370;
    lastUIHeight_ = 100;
}

WahwahAudioProcessor::~WahwahAudioProcessor()
{
}

//==============================================================================
const String WahwahAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int WahwahAudioProcessor::getNumParameters()
{
    return kNumParameters;
}

float WahwahAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kCentreFrequencyParam: return centreFrequency_;
        case kQParam:               return q_;
        default:                    return 0.0f;
    }
}

void WahwahAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kCentreFrequencyParam:
            centreFrequency_ = newValue;
            updateFilter();
            break;
        case kQParam:
            q_ = newValue;
            updateFilter();
            break;
        default:
            break;
    }
}

const String WahwahAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case kCentreFrequencyParam:  return "centre frequency";
        case kQParam:                return "Q";
        default:                     break;
    }
    
    return String::empty;
}

const String WahwahAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String WahwahAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String WahwahAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool WahwahAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool WahwahAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool WahwahAudioProcessor::silenceInProducesSilenceOut() const
{
#if JucePlugin_SilenceInProducesSilenceOut
    return true;
#else
    return false;
#endif
}

double WahwahAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

bool WahwahAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool WahwahAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int WahwahAudioProcessor::getNumPrograms()
{
    return 0;
}

int WahwahAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WahwahAudioProcessor::setCurrentProgram (int index)
{
}

const String WahwahAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void WahwahAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void WahwahAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    // Create as many filters as we have input channels
    numWahFilters_ = getNumInputChannels();
    wahFilters_ = (ResonantLowpassFilter**)malloc(numWahFilters_ * sizeof(ResonantLowpassFilter*));
    if(wahFilters_ == 0)
        numWahFilters_ = 0;
    else {
        for(int i = 0; i < numWahFilters_; i++)
            wahFilters_[i] = new ResonantLowpassFilter;
    }
    
    inverseSampleRate_ = 1.0 / sampleRate;
    
    // Update the filter settings to work with the current parameters and sample rate
    updateFilter();
}

void WahwahAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    for(int i = 0; i < numWahFilters_; i++)
        delete wahFilters_[i];
    if(numWahFilters_ != 0)
        free(wahFilters_);
    numWahFilters_ = 0;
    wahFilters_ = 0;
}

void WahwahAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Helpful information about this block of samples:
    const int numInputChannels = getNumInputChannels();     // How many input channels for our effect?
    const int numOutputChannels = getNumOutputChannels();   // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();          // How many samples in the buffer for this block?
    int channel;
    
    // Go through each channel of audio that's passed in
    
    for (channel = 0; channel < jmin(numInputChannels, numWahFilters_); ++channel)
    {
        // channelData is an array of length numSamples which contains the audio for one channel
        float* channelData = buffer.getWritePointer(channel);
     
        // Run the samples through the IIR filter whose coefficients define the parametric
        // equaliser. See juce_IIRFilter.cpp for the implementation.
        wahFilters_[channel]->processSamples(channelData, numSamples);
    }
    
    // Go through the remaining channels. In case we have more outputs
    // than inputs, or there aren't enough filters, we'll clear any
    // remaining output channels (which could otherwise contain garbage)
    while(channel < numOutputChannels)
    {
        buffer.clear (channel++, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool WahwahAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* WahwahAudioProcessor::createEditor()
{
    return new WahwahAudioProcessorEditor (this);
}

//==============================================================================
void WahwahAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // Create an outer XML element..
    XmlElement xml("C4DMPLUGINSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute("uiWidth", lastUIWidth_);
    xml.setAttribute("uiHeight", lastUIHeight_);
    xml.setAttribute("centreFrequency", centreFrequency_);
    xml.setAttribute("q", q_);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}

void WahwahAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
            
            q_ = (float)xmlState->getDoubleAttribute("q", q_);
            centreFrequency_ = (float)xmlState->getDoubleAttribute("centreFrequency", centreFrequency_);
            inverseSampleRate_ = 1.0 / getSampleRate();
            updateFilter();
        }
    }
}

//==============================================================================
// Update the coefficients of the resonant lowpass filter
void WahwahAudioProcessor::updateFilter()
{
    for(int i = 0; i < numWahFilters_; i++)
        wahFilters_[i]->makeResonantLowpass(inverseSampleRate_, centreFrequency_, q_,
                                            kWahwahFilterGain);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WahwahAudioProcessor();
}
