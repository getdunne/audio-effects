#include "TremoloProcessor.h"
#include "TremoloEditor.h"
#include "TremoloLFO.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TremoloProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* TremoloProcessor::createEditor()
{
    return new TremoloEditor(*this);
}

// Constructor: start off assuming mono input, mono output
TremoloProcessor::TremoloProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::mono(), true)
                     .withOutput ("Output", AudioChannelSet::mono(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier(JucePlugin_Name), TremoloParameters::createParameterLayout())
    , parameters(valueTreeState)
{
}

// Destructor
TremoloProcessor::~TremoloProcessor()
{
}

// Prepare to process audio (always called at least once before processBlock)
void TremoloProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    lfoPhase = 0.0;
    inverseSampleRate = 1.0 / sampleRate;
}

// Audio processing finished; release any allocated memory
void TremoloProcessor::releaseResources()
{
}

// Process one buffer ("block") of data
void TremoloProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    // LFO phase starts at current LFO phase for each channel, advances by deltaPhi for each sample
    float phi = lfoPhase;
    float deltaPhi = float(parameters.lfoFreqHz * inverseSampleRate);

    // apply the same modulation to all input channels for which there is an output channel
    int channelIndex = 0;
    for (; channelIndex < getTotalNumInputChannels(); channelIndex++)
    {
        // restart the phase sequence
        phi = lfoPhase;

        const float* pIn = buffer.getReadPointer(channelIndex);
        float* pOut = buffer.getWritePointer(channelIndex);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            float modAmount = TremoloLFO::getSample(phi, parameters.lfoWaveform);
            *pOut++ = *pIn++ * (1.0f - parameters.modDepth * modAmount);

            // Update LFO phase, keeping in range [0, 1]
            phi += deltaPhi;
            while (phi >= 1.0) phi -= 1.0;
        }
    }

    // update the main LFO phase state variable, ready for the next processBlock() call
    lfoPhase = phi;

    // clear any remaining/excess output channels to zero
    for (; channelIndex < getTotalNumOutputChannels(); channelIndex++)
    {
        buffer.clear(channelIndex, 0, buffer.getNumSamples());
    }
}

// Called by the host when it needs to persist the current plugin state
void TremoloProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void TremoloProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}
