/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Phaser: phasing effect using time-varying allpass filters
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
#include <math.h>

//==============================================================================
PhaserAudioProcessor::PhaserAudioProcessor()
{
    // Set default values:
    baseFrequency_ = 200.0;
    sweepWidth_ = 2000.0;
    depth_ = 1.0;
    feedback_ = 0.0;
    lfoFrequency_ = 0.5;
    waveform_ = kWaveformSine;
    stereo_ = 0;
    
    // Start with no filters (at least until we have some channels)
    allpassFilters_ = 0;
    filtersPerChannel_ = 4;
    totalNumFilters_ = 0;
    lastFilterOutputs_ = 0;
    numLastFilterOutputs_ = 0;
    
    lfoPhase_ = 0.0;
    inverseSampleRate_ = 1.0/44100.0;
    sampleCount_ = 0;
    filterUpdateInterval_ = 8;
    
    lastUIWidth_ = 550;
    lastUIHeight_ = 200;
}

PhaserAudioProcessor::~PhaserAudioProcessor()
{
    deallocateFilters();
}

//==============================================================================
const String PhaserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int PhaserAudioProcessor::getNumParameters()
{
    return kNumParameters;
}

float PhaserAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kBaseFrequencyParam: return baseFrequency_;
        case kSweepWidthParam:    return sweepWidth_;
        case kDepthParam:         return depth_;
        case kFeedbackParam:      return feedback_;
        case kLFOFrequencyParam:  return lfoFrequency_;
        case kFiltersParam:       return (float)filtersPerChannel_;
        case kWaveformParam:      return (float)waveform_;
        case kStereoParam:        return (float)stereo_;
        default:                  return 0.0f;
    }
}

void PhaserAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!

    switch (index)
    {
        case kBaseFrequencyParam:
            baseFrequency_ = newValue;
            break;
        case kSweepWidthParam:
            sweepWidth_ = newValue;
            break;
        case kDepthParam:
            depth_ = newValue;
            break;
        case kFeedbackParam:
            feedback_ = newValue;
            break;
        case kLFOFrequencyParam:
            lfoFrequency_ = newValue;
            break;
        case kFiltersParam:
            if(filtersPerChannel_ != (int)newValue) {
                filtersPerChannel_ = (int)newValue;
                reallocateFilters();
            }
            break;
        case kWaveformParam:
            waveform_ = (int)newValue;
            break;
        case kStereoParam:
            stereo_ = (int)newValue;
            break;
        default:
            break;
    }
}

const String PhaserAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case kBaseFrequencyParam: return "base frequency";
        case kSweepWidthParam:    return "sweep width";
        case kDepthParam:         return "depth";
        case kFeedbackParam:      return "feedback";
        case kLFOFrequencyParam:  return "LFO frequency";
        case kWaveformParam:      return "waveform";
        case kStereoParam:        return "stereo";
        default:                  break;
    }
    
    return String::empty;
}

const String PhaserAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String PhaserAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String PhaserAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool PhaserAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool PhaserAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool PhaserAudioProcessor::silenceInProducesSilenceOut() const
{
#if JucePlugin_SilenceInProducesSilenceOut
    return true;
#else
    return false;
#endif
}

double PhaserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

bool PhaserAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PhaserAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int PhaserAudioProcessor::getNumPrograms()
{
    return 0;
}

int PhaserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PhaserAudioProcessor::setCurrentProgram (int index)
{
}

const String PhaserAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void PhaserAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void PhaserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lfoPhase_ = 0.0;
    inverseSampleRate_ = 1.0/sampleRate;
    sampleCount_ = 0;
    
    const ScopedLock sl (lock_);
    allocateFilters();
}

void PhaserAudioProcessor::releaseResources()
{
    const ScopedLock sl (lock_);
    deallocateFilters();
}

void PhaserAudioProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
    
    lfoPhase_ = 0.0;
    sampleCount_ = 0;
    for(int i = 0; i < numLastFilterOutputs_; i++)
        lastFilterOutputs_[i] = 0.0f;
}


void PhaserAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const ScopedLock sl (lock_);
    
    // Helpful information about this block of samples:
    const int numInputChannels = getNumInputChannels();     // How many input channels for our effect?
    const int numOutputChannels = getNumOutputChannels();   // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();          // How many samples in the buffer for this block?
    float ph, channel0EndPhase = lfoPhase_;
    unsigned int sc;
    
    // Go through each channel of audio that's passed in, applying one or more allpass filters
    // to each. Each channel will be treated identically in a (non-stereo) phaser, but we have
    // to have separate filter objects for each channel since the filters store the last few samples
    // passed through them.
    
    // Filters are stored with all channel 0 filters first, then all channel 1 filters, etc.
    
    for(int channel = 0; channel < numInputChannels; ++channel)
    {
        // channelData is an array of length numSamples which contains the audio for one channel
        float* channelData = buffer.getWritePointer(channel);
        
        ph = lfoPhase_;
        sc = sampleCount_;
        
        // For stereo phasing, keep the channels 90 degrees out of phase with each other
        if(stereo_ != 0 && channel != 0)
            ph = fmodf(ph + 0.25, 1.0);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float out = channelData[sample];
            
            // If feedback is enabled, include the feedback from the last sample in the
            // input of the allpass filter chain. This is actually not accurate to how
            // analog phasers work because there is a sample of delay between output and
            // input, which adds a further phase shift of up to 180 degrees at half the
            // sampling frequency. To truly model an analog phaser with feedback involves
            // modelling a delay-free loop, which is beyond the scope of this example.
            
            if(feedback_ != 0.0 && channel < numLastFilterOutputs_)
                out += feedback_ * lastFilterOutputs_[channel];
            
            // See OnePoleAllpassFilter.cpp for calculation of coefficients and application
            // of filter to audio data. The filter processes the audio buffer in place,
            // putting the output sample in place of the input.
            
            for(int j = 0; j < filtersPerChannel_; ++j)
            {
                // Safety check
                if(channel * filtersPerChannel_ + j >= totalNumFilters_)
                    continue;
                
                // First, update the current allpass filter coefficients depending on the parameter
                // settings and the LFO phase
                
                // Recalculating the filter coefficients is much more expensive than calculating
                // a sample. Only update the coefficients at a fraction of the sample rate; since
                // the LFO moves slowly, the difference won't generally be audible.
                if(sc % filterUpdateInterval_ == 0)
                {
                    allpassFilters_[channel * filtersPerChannel_ + j]->makeAllpass(inverseSampleRate_,
                       baseFrequency_ + sweepWidth_*lfo(ph, waveform_));
                }
                out = allpassFilters_[channel * filtersPerChannel_ + j]->processSingleSampleRaw(out);
            }
            
            if(channel < numLastFilterOutputs_)
                lastFilterOutputs_[channel] = out;
            
            // Add the allpass signal to the output, though maintaining constant level
            // depth = 0 --> input only ; depth = 1 --> evenly balanced input and output
            channelData[sample] = (1.0f-0.5f*depth_)*channelData[sample] + 0.5f*depth_*out;
        
            // Update the LFO phase, keeping it in the range 0-1
            ph += lfoFrequency_*inverseSampleRate_;
            if(ph >= 1.0)
                ph -= 1.0;
            sc++;
        }

        // Use channel 0 only to keep the phase in sync between calls to processBlock()
        // Otherwise quadrature phase on multiple channels will create problems.
        if(channel == 0)
            channel0EndPhase = ph;
    }
    
    lfoPhase_ = channel0EndPhase;
    sampleCount_ = sc;
    
    // Go through the remaining channels. In case we have more outputs
    // than inputs, or there aren't enough filters, we'll clear any
    // remaining output channels (which could otherwise contain garbage)
    for(int channel = numInputChannels; channel < numOutputChannels; ++channel)
    {
        buffer.clear (channel++, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool PhaserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PhaserAudioProcessor::createEditor()
{
    return new PhaserAudioProcessorEditor (this);
}

//==============================================================================
void PhaserAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // Create an outer XML element..
    XmlElement xml("C4DMPLUGINSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute("uiWidth", lastUIWidth_);
    xml.setAttribute("uiHeight", lastUIHeight_);
    xml.setAttribute("baseFrequency_", baseFrequency_);
    xml.setAttribute("feedback", feedback_);
    xml.setAttribute("sweepWidth", sweepWidth_);
    xml.setAttribute("depth", depth_);
    xml.setAttribute("lfoFrequency", lfoFrequency_);
    xml.setAttribute("filtersPerChannel", filtersPerChannel_);
    xml.setAttribute("waveform", waveform_);
    xml.setAttribute("stereo", stereo_);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}

void PhaserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
            
            baseFrequency_ = (float)xmlState->getDoubleAttribute("baseFrequency", baseFrequency_);
            feedback_      = (float)xmlState->getDoubleAttribute("feedback", feedback_);
            sweepWidth_    = (float)xmlState->getDoubleAttribute("sweepWidth", sweepWidth_);
            depth_         = (float)xmlState->getDoubleAttribute("depth", depth_);
            lfoFrequency_  = (float)xmlState->getDoubleAttribute("lfoFrequency", lfoFrequency_);
            filtersPerChannel_ = xmlState->getIntAttribute("filtersPerChannel", filtersPerChannel_);
            waveform_      = xmlState->getIntAttribute("waveform", waveform_);
            stereo_        = xmlState->getIntAttribute("stereo", stereo_);
        }
    }
}

//==============================================================================
// Function for calculating LFO waveforms. Phase runs from 0-1, output is scaled
// from 0 to 1 (note: not -1 to 1 as would be typical of sine).
float PhaserAudioProcessor::lfo(float phase, int waveform)
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
        case kWaveformSawtooth:
            if(phase < 0.5f)
                return 0.5f + phase;
            else
                return phase - 0.5f;
        case kWaveformSine:
        default:
            return 0.5f + 0.5f*sinf(2.0 * M_PI * phase);
    }
}

void PhaserAudioProcessor::allocateFilters()
{
    // Create any filters we need; depends on number of channels and number of
    // filters per channel
    totalNumFilters_ = getNumInputChannels() * filtersPerChannel_;
    if(totalNumFilters_ > 0) {
        allpassFilters_ = (OnePoleAllpassFilter**)malloc(totalNumFilters_ * sizeof(OnePoleAllpassFilter*));
        if(allpassFilters_ == 0)
            totalNumFilters_ = 0;
        else {
            for(int i = 0; i < totalNumFilters_; i++)
                allpassFilters_[i] = new OnePoleAllpassFilter;
        }
    }
    
    numLastFilterOutputs_ = getNumInputChannels();
    lastFilterOutputs_ = (float *)malloc(numLastFilterOutputs_ * sizeof(float));
    for(int i = 0; i < numLastFilterOutputs_; i++)
        lastFilterOutputs_[i] = 0.0f;
    
    // Coefficients of allpass filters will get updated in processBlock()
}

void PhaserAudioProcessor::deallocateFilters()
{
    // Release the filters that were created in prepareToPlay()
    
    for(int i = 0; i < totalNumFilters_; i++)
        delete allpassFilters_[i];
    if(totalNumFilters_ != 0)
        free(allpassFilters_);
    totalNumFilters_ = 0;
    allpassFilters_ = 0;
    
    if(numLastFilterOutputs_ != 0)
        free(lastFilterOutputs_);
    numLastFilterOutputs_ = 0;
    lastFilterOutputs_ = 0;
}

// Release and recreate the filters in one atomic operation:
// the ScopedLock will not let the audio thread run between
// release and allocation
void PhaserAudioProcessor::reallocateFilters()
{
    const ScopedLock sl (lock_);
    deallocateFilters();
    allocateFilters();
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PhaserAudioProcessor();
}
