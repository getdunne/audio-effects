/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "VibratoProcessor.h"
#include "VibratoEditor.h"
#include "LFO_2.h"
#include <math.h>



// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VibratoProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* VibratoProcessor::createEditor()
{
    return new VibratoEditor(*this);
}



const float VibratoProcessor::kMaximumSweepWidth = 0.05;

//==============================================================================
VibratoProcessor::VibratoProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                       )
    , valueTreeState(*this, nullptr, Identifier(JucePlugin_Name), VibratoParameters::createParameterLayout())
    , parameters(valueTreeState)
{
//    frequencyParameter = parameters.getRawParameterValue ("frequency");
//    sweepWidthParameter  = parameters.getRawParameterValue ("sweepWidth");
//    interpolationParameter  = parameters.getRawParameterValue ("interpolation");
//    waveformParameter  = parameters.getRawParameterValue ("waveform");
//    delayBufferLength_ = 1;
//    lfoPhase_ = 0.0;
//    inverseSampleRate_ = 1.0/44100.0;
//
//    // Start the circular buffer pointer at the beginning
//    delayWritePosition_ = 0;
}


VibratoProcessor::~VibratoProcessor()
{
}





void VibratoProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    // Allocate and zero the delay buffer (size will depend on current sample rate)
    // Add 3 extra samples to allow cubic interpolation even at maximum delay
    delayBufferLength_ = (int)(kMaximumSweepWidth*sampleRate) + 3;
    delayBuffer_.setSize(2, delayBufferLength_);
    delayBuffer_.clear();
    lfoPhase_ = 0.0;
}

void VibratoProcessor::releaseResources()
{

}



void VibratoProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();
   
    
    int channel, dpw; // dpr = delay read pointer; dpw = delay write pointer
    float dpr, currentDelay, ph;
    
    // Go through each channel of audio that's passed in. In this example we apply identical
    // effects to each channel, regardless of how many input channels there are. For some effects, like
    // a stereo chorus or panner, you might do something different for each channel.
    
    
    
    
    
    for (channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

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
            
            currentDelay = parameters.sweepWidth*lfo(ph, parameters.lfoWaveform);
            
            // Subtract 3 samples to the delay pointer to make sure we have enough previously written
            // samples to interpolate with
            dpr = fmodf((float)dpw - (float)(currentDelay * getSampleRate()) + (float)delayBufferLength_ - 3.0,
                        (float)delayBufferLength_);
            
            // In this example, the output is the input plus the contents of the delay buffer (weighted by delayMix)
            // The last term implements a tremolo (variable amplitude) on the whole thing.
            
            
            
            Vibrato::processSample(in, parameters.interpolation);
            
            
            
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
            ph += parameters.lfoFreqHz*inverseSampleRate_;
            if(ph >= 1.0)
                ph -= 1.0;
        }
    }
    
    // Having made a local copy of the state variables for each channel, now transfer the result
    // back to the main state variable so they will be preserved for the next call of processBlock()
    
    
    delayWritePosition_ = dpw;
    lfoPhase_ = ph;
    
    
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

}


//==============================================================================
void VibratoProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

void VibratoProcessor::setStateInformation (const void* data, int sizeInBytes)
{

    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}
