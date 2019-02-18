/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Parametric EQ: parametric equaliser adjusting frequency, Q and gain
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


//==============================================================================
ParametricEQAudioProcessor::ParametricEQAudioProcessor()
{
    // Set default values:
    centreFrequency_ = 1000.0;
    q_ = 2.0;
    gainDecibels_ = 0.0;
    
    // Initialise the filters later when we know how many channels
    eqFilters_ = 0;
    numEqFilters_ = 0;
    
    lastUIWidth_ = 550;
    lastUIHeight_ = 100;
}

ParametricEQAudioProcessor::~ParametricEQAudioProcessor()
{
}

//==============================================================================
const String ParametricEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int ParametricEQAudioProcessor::getNumParameters()
{
    return kNumParameters;
}

float ParametricEQAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kCentreFrequencyParam: return centreFrequency_;
        case kQParam:               return q_;
        case kGainDecibelsParam:    return gainDecibels_;
        default:                    return 0.0f;
    }
}

void ParametricEQAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kCentreFrequencyParam:
            centreFrequency_ = newValue;
            updateEQFilter(getSampleRate());
            break;
        case kQParam:
            q_ = newValue;
            updateEQFilter(getSampleRate());
            break;
        case kGainDecibelsParam:
            gainDecibels_ = newValue;
            updateEQFilter(getSampleRate());
            break;
        default:
            break;
    }
}

const String ParametricEQAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case kCentreFrequencyParam:  return "centre frequency";
        case kQParam:                return "Q";
        case kGainDecibelsParam:     return "gain (dB)";
        default:                     break;
    }
    
    return String::empty;
}

const String ParametricEQAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String ParametricEQAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String ParametricEQAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool ParametricEQAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool ParametricEQAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool ParametricEQAudioProcessor::silenceInProducesSilenceOut() const
{
#if JucePlugin_SilenceInProducesSilenceOut
    return true;
#else
    return false;
#endif
}

double ParametricEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

bool ParametricEQAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ParametricEQAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int ParametricEQAudioProcessor::getNumPrograms()
{
    return 0;
}

int ParametricEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ParametricEQAudioProcessor::setCurrentProgram (int index)
{
}

const String ParametricEQAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void ParametricEQAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ParametricEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    // Create as many filters as we have input channels
    numEqFilters_ = getNumInputChannels();
    eqFilters_ = (ParametricEQFilter**)malloc(numEqFilters_ * sizeof(ParametricEQFilter*));
    if(eqFilters_ == 0)
        numEqFilters_ = 0;
    else {
        for(int i = 0; i < numEqFilters_; i++)
            eqFilters_[i] = new ParametricEQFilter;
    }
    
    // Update the filter settings to work with the current parameters and sample rate
    updateEQFilter(sampleRate);
}

void ParametricEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    for(int i = 0; i < numEqFilters_; i++)
        delete eqFilters_[i];
    if(numEqFilters_ != 0)
        free(eqFilters_);
    numEqFilters_ = 0;
    eqFilters_ = 0;
}

void ParametricEQAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Helpful information about this block of samples:
    const int numInputChannels = getNumInputChannels();     // How many input channels for our effect?
    const int numOutputChannels = getNumOutputChannels();   // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();          // How many samples in the buffer for this block?
    int channel;
    
    // Go through each channel of audio that's passed in
    
    for (channel = 0; channel < jmin((int32)numInputChannels, numEqFilters_); ++channel)
    {
        // channelData is an array of length numSamples which contains the audio for one channel
        float* channelData = buffer.getWritePointer(channel);
     
        // Run the samples through the IIR filter whose coefficients define the parametric
        // equaliser. See juce_IIRFilter.cpp for the implementation.
        eqFilters_[channel]->processSamples(channelData, numSamples);
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
bool ParametricEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ParametricEQAudioProcessor::createEditor()
{
    return new ParametricEQAudioProcessorEditor (this);
}

//==============================================================================
void ParametricEQAudioProcessor::getStateInformation (MemoryBlock& destData)
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
    xml.setAttribute("gainDecibels", gainDecibels_);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}

void ParametricEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
            
            centreFrequency_ = (float)xmlState->getDoubleAttribute("centreFrequency", centreFrequency_);
            q_ = (float)xmlState->getDoubleAttribute("q", q_);
            gainDecibels_ = (float)xmlState->getDoubleAttribute("gainDecibels", gainDecibels_);
            updateEQFilter(getSampleRate());
        }
    }
}

//==============================================================================
// Update the coefficients of the parametric equaliser filter
void ParametricEQAudioProcessor::updateEQFilter(float sampleRate)
{
    for(int i = 0; i < numEqFilters_; i++)
        eqFilters_[i]->makeParametric(2.0 * M_PI * centreFrequency_ / sampleRate,
                                      q_, powf(10.0f, gainDecibels_ / 20.0f));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ParametricEQAudioProcessor();
}
