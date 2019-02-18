/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Vibrato: frequency modulation using delay lines
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
#include <math.h>

const float VibratoAudioProcessor::kMaximumSweepWidth = 0.05;

//==============================================================================
VibratoAudioProcessor::VibratoAudioProcessor() : delayBuffer_ (2, 1)
{
    // Set default values:
    sweepWidth_ = .01;
    frequency_ = 2.0;
    waveform_ = kWaveformSine;
    interpolation_ = kInterpolationLinear;
    
    delayBufferLength_ = 1;
    lfoPhase_ = 0.0;
    inverseSampleRate_ = 1.0/44100.0;
    
    // Start the circular buffer pointer at the beginning
    delayWritePosition_ = 0;
    
    lastUIWidth_ = 370;
    lastUIHeight_ = 160;
}

VibratoAudioProcessor::~VibratoAudioProcessor()
{
}

//==============================================================================
const String VibratoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int VibratoAudioProcessor::getNumParameters()
{
    return kNumParameters;
}

float VibratoAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kSweepWidthParam:    return sweepWidth_;
        case kFrequencyParam:     return frequency_;
        case kWaveformParam:      return (float)waveform_;
        case kInterpolationParam: return (float)interpolation_;
        default:                  return 0.0f;
    }
}

void VibratoAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!

    switch (index)
    {
        case kSweepWidthParam:
            sweepWidth_ = newValue;
            break;
        case kFrequencyParam:
            frequency_ = newValue;
            break;
        case kWaveformParam:
            waveform_ = (int)newValue;
            break;
        case kInterpolationParam:
            interpolation_ = (int)newValue;
            break;
        default:
            break;
    }
}

const String VibratoAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case kSweepWidthParam:    return "sweep width";
        case kFrequencyParam:     return "frequency";
        case kWaveformParam:      return "waveform";
        case kInterpolationParam: return "interpolation";
        default:                  break;
    }
    
    return String::empty;
}

const String VibratoAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String VibratoAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String VibratoAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool VibratoAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool VibratoAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool VibratoAudioProcessor::silenceInProducesSilenceOut() const
{
#if JucePlugin_SilenceInProducesSilenceOut
    return true;
#else
    return false;
#endif
}

double VibratoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

bool VibratoAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool VibratoAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int VibratoAudioProcessor::getNumPrograms()
{
    return 0;
}

int VibratoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VibratoAudioProcessor::setCurrentProgram (int index)
{
}

const String VibratoAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void VibratoAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void VibratoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Allocate and zero the delay buffer (size will depend on current sample rate)
    // Add 3 extra samples to allow cubic interpolation even at maximum delay
    delayBufferLength_ = (int)(kMaximumSweepWidth*sampleRate) + 3;
    delayBuffer_.setSize(2, delayBufferLength_);
    delayBuffer_.clear();
    lfoPhase_ = 0.0;
    
    inverseSampleRate_ = 1.0/sampleRate;
}

void VibratoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    
    // The delay buffer will stay in memory until the effect is unloaded.
}

void VibratoAudioProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
    
    delayBuffer_.clear();
}


void VibratoAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Helpful information about this block of samples:
    const int numInputChannels = getNumInputChannels();     // How many input channels for our effect?
    const int numOutputChannels = getNumOutputChannels();   // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();          // How many samples in the buffer for this block?
    
    int channel, dpw; // dpr = delay read pointer; dpw = delay write pointer
    float dpr, currentDelay, ph;
    
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
        
        dpw = delayWritePosition_;
        ph = lfoPhase_;
        
        for (int i = 0; i < numSamples; ++i)
        {
            const float in = channelData[i];
            float interpolatedSample = 0.0;
            
            // Recalculate the read pointer position with respect to the write pointer. A more efficient
            // implementation might increment the read pointer based on the derivative of the LFO without
            // running the whole equation again, but this format makes the operation clearer.
            
            currentDelay = sweepWidth_*lfo(ph, waveform_);
            
            // Subtract 3 samples to the delay pointer to make sure we have enough previously written
            // samples to interpolate with
            dpr = fmodf((float)dpw - (float)(currentDelay * getSampleRate()) + (float)delayBufferLength_ - 3.0,
                        (float)delayBufferLength_);
            
            // In this example, the output is the input plus the contents of the delay buffer (weighted by delayMix)
            // The last term implements a tremolo (variable amplitude) on the whole thing.
  
            if(interpolation_ == kInterpolationLinear)
            {
                // Find the fraction by which the read pointer sits between two
                // samples and use this to adjust weights of the samples
                float fraction = dpr - floorf(dpr);
                int previousSample = (int)floorf(dpr);
                int nextSample = (previousSample + 1) % delayBufferLength_;
                interpolatedSample = fraction*delayData[nextSample]
                + (1.0f-fraction)*delayData[previousSample];
            }
            else if(interpolation_ == kInterpolationCubic)
            {
                // Cubic interpolation will produce cleaner results at the expense
                // of more computation. This code uses the Catmull-Rom variant of
                // cubic interpolation. To reduce the load, calculate a few quantities
                // in advance that will be used several times in the equation:
                
                int sample1 = (int)floorf(dpr);
                int sample2 = (sample1 + 1) % delayBufferLength_;
                int sample3 = (sample2 + 1) % delayBufferLength_;
                int sample0 = (sample1 - 1 + delayBufferLength_) % delayBufferLength_;
                
                float fraction = dpr - floorf(dpr);
                float frsq = fraction*fraction;
                
                float a0 = -0.5f*delayData[sample0] + 1.5f*delayData[sample1]
                            - 1.5f*delayData[sample2] + 0.5f*delayData[sample3];
                float a1 = delayData[sample0] - 2.5f*delayData[sample1]
                            + 2.0f*delayData[sample2] - 0.5f*delayData[sample3];
                float a2 = -0.5f*delayData[sample0] + 0.5f*delayData[sample2];
                float a3 = delayData[sample1];
                
                interpolatedSample = a0*fraction*frsq + a1*frsq + a2*fraction + a3;
            }
            else // Nearest neighbour interpolation
            {
                // Find the nearest input sample by rounding the fractional index to the
                // nearest integer. It's possible this will round it to the end of the buffer,
                // in which case we need to roll it back to the beginning.
                int closestSample = (int)floorf(dpr + 0.5);
                if(closestSample == delayBufferLength_)
                    closestSample = 0;
                interpolatedSample = delayData[closestSample];
            }
            
            // Store the current information in the delay buffer. With feedback, what we read is
            // included in what gets stored in the buffer, otherwise it's just a simple delay line
            // of the input signal.
            
            delayData[dpw] = in;
            
            // Increment the write pointer at a constant rate. The read pointer will move at different
            // rates depending on the settings of the LFO, the delay and the sweep width.
            
            if (++dpw >= delayBufferLength_)
                dpw = 0;

            // Store the output sample in the buffer, replacing the input. In the vibrato effect,
            // the delaye sample is the only component of the output (no mixing with the dry signal)
            channelData[i] = interpolatedSample;

            // Update the LFO phase, keeping it in the range 0-1
            ph += frequency_*inverseSampleRate_;
            if(ph >= 1.0)
                ph -= 1.0;
        }
    }
    
    // Having made a local copy of the state variables for each channel, now transfer the result
    // back to the main state variable so they will be preserved for the next call of processBlock()
    
    delayWritePosition_ = dpw;
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
bool VibratoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* VibratoAudioProcessor::createEditor()
{
    return new VibratoAudioProcessorEditor (this);
}

//==============================================================================
void VibratoAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // Create an outer XML element..
    XmlElement xml("C4DMPLUGINSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute("uiWidth", lastUIWidth_);
    xml.setAttribute("uiHeight", lastUIHeight_);
    xml.setAttribute("sweepWidth", sweepWidth_);
    xml.setAttribute("frequency", frequency_);
    xml.setAttribute("waveform", waveform_);
    xml.setAttribute("interpolation", interpolation_);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}

void VibratoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
            
            sweepWidth_ = (float)xmlState->getDoubleAttribute("sweepWidth", sweepWidth_);
            frequency_  = (float)xmlState->getDoubleAttribute("frequency", frequency_);
            waveform_   = xmlState->getIntAttribute("waveform", waveform_);
            interpolation_ = xmlState->getIntAttribute("interpolation", interpolation_);
        }
    }
}

//==============================================================================
// Function for calculating LFO waveforms. Phase runs from 0-1, output is scaled
// from 0 to 1 (note: not -1 to 1 as would be typical of sine).
float VibratoAudioProcessor::lfo(float phase, int waveform)
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
        case kWaveformInverseSawtooth:
            if(phase < 0.5f)
                return 0.5f - phase;
            else
                return 1.5f - phase;
        case kWaveformSine:
        default:
            return 0.5f + 0.5f*sinf(2.0 * M_PI * phase);
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VibratoAudioProcessor();
}
