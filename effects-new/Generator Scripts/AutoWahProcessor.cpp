#include "AutoWahProcessor.h"
#include "AutoWahEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutoWahProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* AutoWahProcessor::createEditor()
{
    return new AutoWahEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
AutoWahProcessor::AutoWahProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("AutoWah"), AutoWahParameters::createParameterLayout())
    , parameters(valueTreeState)
{
}

// Destructor
AutoWahProcessor::~AutoWahProcessor()
{
}

// Prepare to process audio (always called at least once before processBlock)
void AutoWahProcessor::prepareToPlay (double /*sampleRate*/, int /*maxSamplesPerBlock*/)
{
}

// Audio processing finished; release any allocated memory
void AutoWahProcessor::releaseResources()
{
}

// Process one buffer ("block") of data
void AutoWahProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;
}

// Called by the host when it needs to persist the current plugin state
void AutoWahProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void AutoWahProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}