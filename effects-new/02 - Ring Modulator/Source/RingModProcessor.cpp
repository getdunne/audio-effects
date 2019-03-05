#include "RingModProcessor.h"
#include "RingModEditor.h"
#include "RingModLFO.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RingModProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* RingModProcessor::createEditor()
{
    return new RingModEditor(*this);
}

// Constructor: start off assuming mono input, mono output
RingModProcessor::RingModProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::mono(), true)
                     .withOutput ("Output", AudioChannelSet::mono(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier(JucePlugin_Name), RingModParameters::createParameterLayout())
    , parameters(valueTreeState)
{
}

// Destructor
RingModProcessor::~RingModProcessor()
{
}

// Prepare to process audio (always called at least once before processBlock)
void RingModProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    carrierPhase = 0.0f;
    lfoPhase = 0.0f;
    inverseSampleRate = 1.0 / sampleRate;
}

// Audio processing finished; release any allocated memory
void RingModProcessor::releaseResources()
{
}

// Process one buffer ("block") of data
void RingModProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    // local copies of state variables carrierPhase and lfoPhase
    float cphi = carrierPhase;
    float lphi = lfoPhase;
    float dlphi = float(parameters.lfoFreqHz * inverseSampleRate);

    // apply the same modulation to all input channels for which there is an output channel
    int channelIndex = 0;
    for (; channelIndex < getTotalNumInputChannels(); channelIndex++)
    {
        // restart the phase sequence
        cphi = carrierPhase;
        lphi = lfoPhase;

        const float* pIn = buffer.getReadPointer(channelIndex);
        float* pOut = buffer.getWritePointer(channelIndex);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            // Carrier oscillator is a simple sine wave
            const float twoPi = 6.283185f;
            float carrier = sin(twoPi * cphi);

            // Ring modulation is just simple multiplication
            *pOut++ = *pIn++ * carrier;

            // Update carrier phase with FM, keeping in range [0, 1]
            float lfo = RingModLFO::getSample(lphi, parameters.lfoWaveform);
            float deltaCarrierHz = parameters.lfoWidthHz * lfo;
            float dcphi = float((parameters.carrierFreqHz + deltaCarrierHz) * inverseSampleRate);
            cphi += dcphi;
            while (cphi >= 1.0) cphi -= 1.0;

            // Update LFO phase, keeping in range [0, 1]
            lphi += dlphi;
            while (lphi >= 1.0) lphi -= 1.0;
        }
    }

    // update the main phase state variables, ready for the next processBlock() call
    carrierPhase = cphi;
    lfoPhase = lphi;

    // clear any remaining/excess output channels to zero
    for (; channelIndex < getTotalNumOutputChannels(); channelIndex++)
    {
        buffer.clear(channelIndex, 0, buffer.getNumSamples());
    }
}

// Called by the host when it needs to persist the current plugin state
void RingModProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void RingModProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}
