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

//    delayBufferLength_ = 1;

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
    delayWritePosition_ = 0;
    delayBufferLength_ = (int)(kMaximumSweepWidth*sampleRate) + 3;
    delayBuffer_.setSize(2, delayBufferLength_);
    delayBuffer_.clear();
    lfoPhase_ = 0.0;
    inverseSampleRate_ = 1.0/sampleRate;
}

void VibratoProcessor::releaseResources()
{

}



void VibratoProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& )
{
    ScopedNoDenormals noDenormals;

     // apply the same vibrato to all input channels for which there is an output channel
    int dpw;
    float ph;
    
    int channelIndex = 0;
    for (; channelIndex < getTotalNumInputChannels(); channelIndex++)
    {
        auto* channelData = buffer.getWritePointer (channelIndex);
        float* delayData = delayBuffer_.getWritePointer (jmin (channelIndex, delayBuffer_.getNumChannels() - 1));
        
        
        dpw = delayWritePosition_;
        ph = lfoPhase_;
        
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            const float in = channelData[i];
            float interpolatedSample = 0.0;
            
            // Recalculate the read pointer position with respect to the write pointer. A more efficient
            // implementation might increment the read pointer based on the derivative of the LFO without
            // running the whole equation again, but this format makes the operation clearer.
            
            float currentDelay = parameters.sweepWidth*LFO_2::getSample(ph, parameters.lfoWaveform);
            
            // Subtract 3 samples to the delay pointer to make sure we have enough previously written
            // samples to interpolate with
            float dpr = fmodf((float)dpw - (float)(currentDelay * getSampleRate()) + (float)delayBufferLength_ - 3.0,
                        (float)delayBufferLength_);
            

            Vibrato::processSample(in, parameters.interpolation, dpr, delayData, delayBufferLength_, interpolatedSample );
            
            
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
    
    for (; channelIndex < getTotalNumOutputChannels(); channelIndex++)
    {
        buffer.clear (channelIndex, 0, buffer.getNumSamples());
    }
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
