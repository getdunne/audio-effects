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

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <math.h>

//==============================================================================
RingModulatorAudioProcessor::RingModulatorAudioProcessor()
{
    // Set default values:
    carrierFrequency_ = 100.0;
    sweepWidth_ = 0.0;
    lfoFrequency_ = 1.0;
    waveform_ = kWaveformSine;
    
    lfoPhase_ = carrierPhase_ = 0.0;
    inverseSampleRate_ = 1.0/44100.0;
    
    lastUIWidth_ = 370;
    lastUIHeight_ = 160;
}

RingModulatorAudioProcessor::~RingModulatorAudioProcessor()
{
}

//==============================================================================
const String RingModulatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int RingModulatorAudioProcessor::getNumParameters()
{
    return kNumParameters;
}

float RingModulatorAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kCarrierFrequencyParam: return carrierFrequency_;
        case kSweepWidthParam:       return sweepWidth_;
        case kLFOFrequencyParam:     return lfoFrequency_;
        case kWaveformParam:         return (float)waveform_;
        default:                     return 0.0f;
    }
}

void RingModulatorAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!

    switch (index)
    {
        case kCarrierFrequencyParam:
            carrierFrequency_ = newValue;
            break;
        case kSweepWidthParam:
            sweepWidth_ = newValue;
            break;
        case kLFOFrequencyParam:
            lfoFrequency_ = newValue;
            break;
        case kWaveformParam:
            waveform_ = (int)newValue;
            break;
        default:
            break;
    }
}

const String RingModulatorAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case kCarrierFrequencyParam: return "carrier frequency";
        case kSweepWidthParam:       return "LFO sweep width";
        case kLFOFrequencyParam:     return "LFO frequency";
        case kWaveformParam:         return "LFO waveform";
        default:                     break;
    }
    
    return String::empty;
}

const String RingModulatorAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String RingModulatorAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String RingModulatorAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool RingModulatorAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool RingModulatorAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool RingModulatorAudioProcessor::silenceInProducesSilenceOut() const
{
#if JucePlugin_SilenceInProducesSilenceOut
    return true;
#else
    return false;
#endif
}

double RingModulatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

bool RingModulatorAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool RingModulatorAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int RingModulatorAudioProcessor::getNumPrograms()
{
    return 0;
}

int RingModulatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RingModulatorAudioProcessor::setCurrentProgram (int index)
{
}

const String RingModulatorAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void RingModulatorAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void RingModulatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Calculations that happen before play begins. Pretty simple in this effect, just
    // reset the previous state.
    lfoPhase_ = 0.0;
    carrierPhase_ = 0.0;
    inverseSampleRate_ = 1.0/sampleRate;
}

void RingModulatorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void RingModulatorAudioProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
    
    lfoPhase_ = 0.0;
    carrierPhase_ = 0.0;
}


void RingModulatorAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Helpful information about this block of samples:
    const int numInputChannels = getNumInputChannels();     // How many input channels for our effect?
    const int numOutputChannels = getNumOutputChannels();   // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();          // How many samples in the buffer for this block?
    
    int channel;
    float cph, lph;
    
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
        
        cph = carrierPhase_;
        lph = lfoPhase_;
        
        for (int i = 0; i < numSamples; ++i)
        {
            const float in = channelData[i];

            // Ring modulation is easy! Just multiply the waveform by a periodic carrier
            channelData[i] = in * sinf(2.0 * M_PI * cph);

            // Update the carrier and LFO phases, keeping them in the range 0-1
            lph += lfoFrequency_*inverseSampleRate_;
            if(lph >= 1.0)
                lph -= 1.0;
            cph += (carrierFrequency_ + sweepWidth_*lfo(lfoPhase_, waveform_))*inverseSampleRate_;
            if(cph >= 1.0)
                cph -= 1.0;
        }
    }
    
    // Having made a local copy of the state variables for each channel, now transfer the result
    // back to the main state variable so they will be preserved for the next call of processBlock()
    
    carrierPhase_ = cph;
    lfoPhase_ = lph;
    
    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = numInputChannels; i < numOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool RingModulatorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* RingModulatorAudioProcessor::createEditor()
{
    return new RingModulatorAudioProcessorEditor (this);
}

//==============================================================================
void RingModulatorAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // Create an outer XML element..
    XmlElement xml("C4DMPLUGINSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute("uiWidth", lastUIWidth_);
    xml.setAttribute("uiHeight", lastUIHeight_);
    xml.setAttribute("carrierFrequency", carrierFrequency_);
    xml.setAttribute("sweepWidth", sweepWidth_);
    xml.setAttribute("lfoFrequency", lfoFrequency_);
    xml.setAttribute("waveform", waveform_);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}

void RingModulatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
            
            carrierFrequency_  = (float)xmlState->getDoubleAttribute("carrierFrequency", carrierFrequency_);
            sweepWidth_ = (float)xmlState->getDoubleAttribute("sweepWidth", sweepWidth_);
            lfoFrequency_  = (float)xmlState->getDoubleAttribute("lfoFrequency", lfoFrequency_);
            waveform_   = xmlState->getIntAttribute("waveform", waveform_);
        }
    }
}

//==============================================================================
// Function for calculating LFO waveforms. Phase runs from 0-1, output is scaled
// from -1 to 1 (note: not 0 to 1 as in delay-based effects)
float RingModulatorAudioProcessor::lfo(float phase, int waveform)
{
    switch(waveform)
    {
        case kWaveformTriangle:
            if(phase < 0.25f)
                return 4.0f*phase;
            else if(phase < 0.75f)
                return 1.0f - 4.0f*(phase - 0.25f);
            else
                return -1.0f + 4.0f*(phase - 0.75f);
        case kWaveformSquare:
            if(phase < 0.5f)
                return 1.0f;
            else
                return -1.0f;
        case kWaveformSawtooth:
            if(phase < 0.5f)
                return 2.0f*phase;
            else
                return 2.0f*phase - 2.0f;
        case kWaveformInverseSawtooth:
            if(phase < 0.5f)
                return -2.0f*phase;
            else
                return 2.0f - 2.0f*phase;
        case kWaveformSine:
        default:
            return sinf(2.0 * M_PI * phase);
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RingModulatorAudioProcessor();
}
