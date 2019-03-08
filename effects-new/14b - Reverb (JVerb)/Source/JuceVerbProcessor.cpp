#include "JuceVerbProcessor.h"
#include "JuceVerbEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JuceVerbProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* JuceVerbProcessor::createEditor()
{
    return new JuceVerbEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
JuceVerbProcessor::JuceVerbProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("JuceVerb"), JuceVerbParameters::createParameterLayout())
    , parameters(valueTreeState, this)
{
}

// Destructor
JuceVerbProcessor::~JuceVerbProcessor()
{
}

bool JuceVerbProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    int ins = layouts.getMainInputChannels();
    int outs = layouts.getMainOutputChannels();

    // accept mono in/out or stereo in/out only
    return ((ins == 2 && outs == 2) || (ins == 1 && outs == 1));
}

void JuceVerbProcessor::parameterChanged(const String& paramID, float newValue)
{
    Reverb::Parameters params = reverb.getParameters();

    if (paramID == parameters.roomSizeID)
        params.roomSize = newValue;
    else if (paramID == parameters.dampingID)
        params.damping = newValue;
    else if (paramID == parameters.wetLevelID)
        params.wetLevel = newValue;
    else if (paramID == parameters.dryLevelID)
        params.dryLevel = newValue;
    else if (paramID == parameters.widthID)
        params.width = newValue;
    else if (paramID == parameters.freezeModeID)
        params.freezeMode = newValue;

    reverb.setParameters(params);
}

// Prepare to process audio (always called at least once before processBlock)
void JuceVerbProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    reverb.setSampleRate(sampleRate);
}

// Audio processing finished; release any allocated memory
void JuceVerbProcessor::releaseResources()
{
}

// Process one buffer ("block") of data
void JuceVerbProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    auto numChannels = buffer.getNumChannels();

    if (numChannels == 1)
        reverb.processMono(buffer.getWritePointer(0), buffer.getNumSamples());
    else
        reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());

    for (int ch = 2; ch < numChannels; ++ch)
        buffer.clear(ch, 0, buffer.getNumSamples());
}

// Called by the host when it needs to persist the current plugin state
void JuceVerbProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void JuceVerbProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}