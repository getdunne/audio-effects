#include "{{ projectName }}Processor.h"
#include "{{ projectName }}Editor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new {{ projectName }}Processor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* {{ projectName }}Processor::createEditor()
{
    return new {{ projectName }}Editor(*this);
}

// Constructor: start off assuming stereo input, stereo output
{{ projectName }}Processor::{{ projectName }}Processor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("{{ projectName }}"), {{ projectName }}Parameters::createParameterLayout())
    , parameters(valueTreeState)
{
}

// Destructor
{{ projectName }}Processor::~{{ projectName }}Processor()
{
}

// Prepare to process audio (always called at least once before processBlock)
void {{ projectName }}Processor::prepareToPlay (double /*sampleRate*/, int /*maxSamplesPerBlock*/)
{
}

// Audio processing finished; release any allocated memory
void {{ projectName }}Processor::releaseResources()
{
}

// Process one buffer ("block") of data
void {{ projectName }}Processor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;
}

// Called by the host when it needs to persist the current plugin state
void {{ projectName }}Processor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void {{ projectName }}Processor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}
