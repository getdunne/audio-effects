#include "ParametricEQProcessor.h"
#include "ParametricEQEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ParametricEQProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* ParametricEQProcessor::createEditor()
{
    return new ParametricEQEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
ParametricEQProcessor::ParametricEQProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("ParametricEQ"), ParametricEQParameters::createParameterLayout())
    , parameters(valueTreeState, this)
{
}

// Destructor
ParametricEQProcessor::~ParametricEQProcessor()
{
}

void ParametricEQProcessor::parameterChanged(const String&, float)
{
    // all parameters affect the filters
    updateFilters();

    // inform our GUI that the filter bandwidth may have changed
    sendChangeMessage();
}


void ParametricEQProcessor::updateFilters()
{
#define TWOPI_D 6.283185307

    for (auto& filter : filters)
    {
        filter->makeParametric(TWOPI_D * parameters.centreFreqHz / sampleRateHz,
                               parameters.filterQ, parameters.linearGain);
    }
}

// Prepare to process audio (always called at least once before processBlock)
void ParametricEQProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    sampleRateHz = sampleRate;

    // create as many identical filters as there are input channels
    for (int i = 0; i < getTotalNumInputChannels(); i++)
        filters.add(new ParametricEQFilter);

    // Update the filter settings to work with the current parameters and sample rate
    updateFilters();
}

// Audio processing finished; release any allocated memory
void ParametricEQProcessor::releaseResources()
{
    filters.clear();
}

// Process one buffer ("block") of data
void ParametricEQProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    int numInputChannels = getTotalNumInputChannels();
    int numFilters = filters.size();    // should be the same
    int numOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();

    int ch = 0;
    for (; ch < jmin(numInputChannels, numFilters); ch++)
    {
        // Run the samples through the IIR filter whose coefficients define the parametric
        // equaliser. See juce_IIRFilter.cpp for the implementation.
        filters[ch]->processSamples(buffer.getWritePointer(ch), numSamples);
    }

    // Go through the remaining channels. In case we have more outputs
    // than inputs, or there aren't enough filters, we'll clear any
    // remaining output channels (which could otherwise contain garbage)
    while (ch < numOutputChannels)
    {
        buffer.clear(ch++, 0, numSamples);
    }
}

// Called by the host when it needs to persist the current plugin state
void ParametricEQProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void ParametricEQProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}