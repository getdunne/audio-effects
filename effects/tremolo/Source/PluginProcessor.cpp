/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Tremolo: amplitude modulation using a low-frequency oscillator
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

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <math.h>

//==============================================================================
TremoloAudioProcessor::TremoloAudioProcessor()
{
    // Set default values:
    frequency_ = 2.0;
    depth_ = 1.0;
    waveform_ = kWaveformSine;
    
    lfoPhase_ = 0.0;
    inverseSampleRate_ = 1.0/44100.0;
    
    lastUIWidth_ = 370;
    lastUIHeight_ = 140;
}

TremoloAudioProcessor::~TremoloAudioProcessor()
{
}

//==============================================================================
const String TremoloAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int TremoloAudioProcessor::getNumParameters()
{
    return kNumParameters;
}

float TremoloAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kFrequencyParam:     return frequency_;
        case kDepthParam:         return depth_;
        case kWaveformParam:      return (float)waveform_;
        default:                  return 0.0f;
    }
}

void TremoloAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!

    switch (index)
    {
        case kFrequencyParam:
            frequency_ = newValue;
            break;
        case kDepthParam:
            depth_ = newValue;
            break;
        case kWaveformParam:
            waveform_ = (int)newValue;
            break;
        default:
            break;
    }
}

const String TremoloAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case kFrequencyParam:     return "frequency";
        case kDepthParam:         return "depth";
        case kWaveformParam:      return "waveform";
        default:                  break;
    }
    
    return String::empty;
}

const String TremoloAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String TremoloAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String TremoloAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool TremoloAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool TremoloAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool TremoloAudioProcessor::silenceInProducesSilenceOut() const
{
#if JucePlugin_SilenceInProducesSilenceOut
    return true;
#else
    return false;
#endif
}

double TremoloAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

bool TremoloAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool TremoloAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int TremoloAudioProcessor::getNumPrograms()
{
    return 0;
}

int TremoloAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TremoloAudioProcessor::setCurrentProgram (int index)
{
}

const String TremoloAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void TremoloAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void TremoloAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Calculations that happen before play begins. Pretty simple in this effect, just
    // reset the previous state.
    lfoPhase_ = 0.0;
    inverseSampleRate_ = 1.0/sampleRate;
}

void TremoloAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void TremoloAudioProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
    
    lfoPhase_ = 0.0;
}


void TremoloAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Helpful information about this block of samples:
    const int numInputChannels = getNumInputChannels();     // How many input channels for our effect?
    const int numOutputChannels = getNumOutputChannels();   // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();          // How many samples in the buffer for this block?
    
    int channel;
    float ph;
    
    // Go through each channel of audio that's passed in. In this example we apply identical
    // effects to each channel, regardless of how many input channels there are. For some effects, like
    // a stereo chorus or panner, you might do something different for each channel.
    
    for (channel = 0; channel < numInputChannels; ++channel)
    {
        // channelData is an array of length numSamples which contains the audio for one channel
        float* channelData = buffer.getWritePointer(channel);
        
        // Make a temporary copy of any state variables declared in PluginProcessor.h which need to be
        // maintained between calls to processBlock(). Each channel needs to be processed identically
        // which means that the activity of processing one channel can't affect the state variable for
        // the next channel.
        
        ph = lfoPhase_;
        
        for (int i = 0; i < numSamples; ++i)
        {
            const float in = channelData[i];

            // Ring modulation is easy! Just multiply the waveform by a periodic carrier
            channelData[i] = in * (1.0f - depth_*lfo(ph, waveform_));

            // Update the carrier and LFO phases, keeping them in the range 0-1
            ph += frequency_*inverseSampleRate_;
            if(ph >= 1.0)
                ph -= 1.0;
        }
    }
    
    // Having made a local copy of the state variables for each channel, now transfer the result
    // back to the main state variable so they will be preserved for the next call of processBlock()
    
    lfoPhase_ = ph;
    
    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = numInputChannels; i < numOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool TremoloAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* TremoloAudioProcessor::createEditor()
{
    return new TremoloAudioProcessorEditor (this);
}

//==============================================================================
void TremoloAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // Create an outer XML element..
    XmlElement xml("C4DMPLUGINSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute("uiWidth", lastUIWidth_);
    xml.setAttribute("uiHeight", lastUIHeight_);
    xml.setAttribute("frequency", frequency_);
    xml.setAttribute("depth", depth_);
    xml.setAttribute("waveform", waveform_);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}

void TremoloAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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

            depth_ = (float)xmlState->getDoubleAttribute("depth", depth_);
            frequency_  = (float)xmlState->getDoubleAttribute("frequency", frequency_);
            waveform_   = xmlState->getIntAttribute("waveform", waveform_);
        }
    }
}

//==============================================================================
// Function for calculating LFO waveforms. Phase runs from 0-1, output is scaled
// from 0 to 1 (note: not -1 to 1 as would be typical of sine).
float TremoloAudioProcessor::lfo(float phase, int waveform)
{
    switch(waveform)
    {
        case kWaveformTriangle:
            if(phase < 0.25f)
                return 0.5f + 2.0f*phase;
            else if(phase < 0.75f)
                return 1.0f - 2.0f*(phase - 0.25f);
            else
                return 2.0f*(phase-0.75f);
        case kWaveformSquare:
            if(phase < 0.5f)
                return 1.0f;
            else
                return 0.0f;
        case kWaveformSquareSlopedEdges:
            if(phase < 0.48f)
                return 1.0f;
            else if(phase < 0.5f)
                return 1.0f - 50.0f*(phase - 0.48f);
            else if(phase < 0.98f)
                return 0.0f;
            else
                return 50.0f*(phase - 0.98f);
        case kWaveformSine:
        default:
            return 0.5f + 0.5f*sinf(2.0 * M_PI * phase);
    }
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TremoloAudioProcessor();
}
