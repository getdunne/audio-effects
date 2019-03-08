#include "ReverbProcessor.h"
#include "ReverbEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverbProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* ReverbProcessor::createEditor()
{
    return new ReverbEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
ReverbProcessor::ReverbProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", AudioChannelSet::stereo(), true)
        .withOutput("Output", AudioChannelSet::stereo(), true)
    )
    , valueTreeState(*this, nullptr, Identifier("Reverb"), ReverbParameters::createParameterLayout())
    , parameters(valueTreeState, this)
{
    mverb.setParameter(mverb.DAMPINGFREQ, parameters.dampingDefault);
    mverb.setParameter(mverb.DENSITY, parameters.densityDefault);
    mverb.setParameter(mverb.BANDWIDTHFREQ, parameters.bandwidthDefault);
    mverb.setParameter(mverb.DECAY, parameters.decayDefault);
    mverb.setParameter(mverb.PREDELAY, parameters.predelayDefault);
    mverb.setParameter(mverb.SIZE, parameters.roomSizeDefault);
    mverb.setParameter(mverb.GAIN, 1.0f);   // leave this fixed at 100%
    mverb.setParameter(mverb.MIX, parameters.dryWetMixDefault);
    mverb.setParameter(mverb.EARLYMIX, parameters.earlyLateMixDefault);
}

// Destructor
ReverbProcessor::~ReverbProcessor()
{
}

void ReverbProcessor::parameterChanged(const String& paramID, float newValue)
{
    if (paramID == ReverbParameters::dampingID)
        mverb.setParameter(mverb.DAMPINGFREQ, newValue);
    else if (paramID == ReverbParameters::densityID)
        mverb.setParameter(mverb.DENSITY, newValue);
    else if (paramID == ReverbParameters::bandwidthID)
        mverb.setParameter(mverb.BANDWIDTHFREQ, newValue);
    else if (paramID == ReverbParameters::decayID)
        mverb.setParameter(mverb.DECAY, newValue);
    else if (paramID == ReverbParameters::predelayID)
        mverb.setParameter(mverb.PREDELAY, newValue);
    else if (paramID == ReverbParameters::roomSizeID)
        mverb.setParameter(mverb.SIZE, newValue);
    else if (paramID == ReverbParameters::dryWetMixID)
        mverb.setParameter(mverb.MIX, newValue);
    else if (paramID == ReverbParameters::earlyLateMixID)
        mverb.setParameter(mverb.EARLYMIX, newValue);
}

bool ReverbProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    int ins = layouts.getMainInputChannels();
    int outs = layouts.getMainOutputChannels();

    // accept mono in/out or stereo in/out only
    return ((ins == 2 && outs == 2) || (ins == 1 && outs == 1));
}

// Prepare to process audio (always called at least once before processBlock)
void ReverbProcessor::prepareToPlay(double sampleRate, int maxSamplesPerBlock)
{
    if (maxSamplesPerBlock != tempInput.getNumSamples())
    {
        mverb.reset();
        mverb.setSampleRate(float(sampleRate));

        tempInput.setSize(2, maxSamplesPerBlock);
        tempOutput.setSize(2, maxSamplesPerBlock);
    }
}

// Audio processing finished; release any allocated memory
void ReverbProcessor::releaseResources()
{
}

// Process one buffer ("block") of data
void ReverbProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    for (int channel = 0; channel < numChannels; channel++)
    {
        tempInput.copyFrom(channel, 0, buffer, channel, 0, numSamples);
    }

    float** input = tempInput.getArrayOfWritePointers();
    float** output = tempOutput.getArrayOfWritePointers();

    mverb.process(input, output, numSamples);

    for (int channel = 0; channel < numChannels; channel++)
    {
        buffer.copyFrom(channel, 0, output[channel], numSamples);
    }
}

// Called by the host when it needs to persist the current plugin state
void ReverbProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void ReverbProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}