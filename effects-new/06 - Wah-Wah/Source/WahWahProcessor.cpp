#include "WahWahProcessor.h"
#include "WahWahEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WahWahProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* WahWahProcessor::createEditor()
{
    return new WahWahEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
WahWahProcessor::WahWahProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("WahWah"), WahWahParameters::createParameterLayout())
    , parameters(valueTreeState, this)
    , inverseSampleRate(1.0 / 44100.0)  // sensible default
{
}

// Destructor
WahWahProcessor::~WahWahProcessor()
{
}

void WahWahProcessor::updateFilters()
{
    // The filter will produce a resonant peak of amplitude Q; bring everything
    // down somewhat to compensate, though try to maintain some perceptual balance
    // of being similar loudness. (This factor has been chosen somewhat arbitrarily.)
    const double kWahwahFilterGain = 0.5;

    for (auto& filter : filters)
    {
        filter->makeResonantLowpass(inverseSampleRate, parameters.centreFreqHz, parameters.filterQ,
                                    kWahwahFilterGain);
    }
}

// Prepare to process audio (always called at least once before processBlock)
void WahWahProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    inverseSampleRate = 1.0 / sampleRate;

    // create as many identical filters as there are input channels
    for (int i = 0; i < getTotalNumInputChannels(); i++)
        filters.add(new ResonantLowpassFilter);

    // Update the filter settings to work with the current parameters and sample rate
    updateFilters();
}

// Audio processing finished; release any allocated memory
void WahWahProcessor::releaseResources()
{
    filters.clear();
}

// Process one buffer ("block") of data
void WahWahProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
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
void WahWahProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void WahWahProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}