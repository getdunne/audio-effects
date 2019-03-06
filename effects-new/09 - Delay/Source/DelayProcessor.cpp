#include "DelayProcessor.h"
#include "DelayEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* DelayProcessor::createEditor()
{
    return new DelayEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
DelayProcessor::DelayProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("Delay"), DelayParameters::createParameterLayout())
    , parameters(valueTreeState, this)
    , delayReadPosition(0)
    , delayWritePosition(0)
{
}

// Destructor
DelayProcessor::~DelayProcessor()
{
}

void DelayProcessor::parameterChanged(const String&, float)
{
    delayReadPosition = (int)(delayWritePosition - (parameters.delaySec * getSampleRate())
                                                 + delayBufferLength) % delayBufferLength;
}

// Prepare to process audio (always called at least once before processBlock)
void DelayProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    // Allocate and zero the delay buffer (size will depend on current sample rate)
    // Sanity check the result so we don't end up with any zero-length calculations
    delayBufferLength = (int)(2.0*sampleRate);
    if (delayBufferLength < 1) delayBufferLength = 1;
    delayBuffer.setSize(2, delayBufferLength);
    delayBuffer.clear();

    // This method gives us the sample rate. Use this to figure out what the delay position
    // offset should be (since it is specified in seconds, and we need to convert it to a number
    // of samples)
    delayReadPosition = (int)(delayWritePosition - (parameters.delaySec * getSampleRate())
                                                 + delayBufferLength) % delayBufferLength;
}

// Audio processing finished; release any allocated memory
void DelayProcessor::releaseResources()
{
}

// Process one buffer ("block") of data
void DelayProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    // Helpful information about this block of samples:
    const int numInputChannels = getTotalNumInputChannels();    // How many input channels for our effect?
    const int numOutputChannels = getTotalNumOutputChannels();  // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();              // How many samples in the buffer for this block?

    int channel, dpr, dpw; // dpr = delay read pointer; dpw = delay write pointer

    // Go through each channel of audio that's passed in. In this example we apply identical
    // effects to each channel, regardless of how many input channels there are. For some effects, like
    // a stereo chorus or panner, you might do something different for each channel.
    for (channel = 0; channel < numInputChannels; ++channel)
    {
        // channelData is an array of length numSamples which contains the audio for one channel
        float* channelData = buffer.getWritePointer(channel);

        // delayData is the circular buffer for implementing delay on this channel
        float* delayData = delayBuffer.getWritePointer(jmin(channel, delayBuffer.getNumChannels() - 1));

        // Make a temporary copy of any state variables declared in PluginProcessor.h which need to be
        // maintained between calls to processBlock(). Each channel needs to be processed identically
        // which means that the activity of processing one channel can't affect the state variable for
        // the next channel.
        dpr = delayReadPosition;
        dpw = delayWritePosition;

        for (int i = 0; i < numSamples; ++i)
        {
            const float in = channelData[i];
            float out = 0.0;

            // In this example, the output is the input plus the contents of the delay buffer (weighted by delayMix)
            // The last term implements a tremolo (variable amplitude) on the whole thing.

            out = (parameters.dryLevel * in + parameters.wetLevel * delayData[dpr]);

            // Store the current information in the delay buffer. delayData[dpr] is the delay sample we just read,
            // i.e. what came out of the buffer. delayData[dpw] is what we write to the buffer, i.e. what goes in

            delayData[dpw] = in + (delayData[dpr] * parameters.feedback);

            if (++dpr >= delayBufferLength) dpr = 0;
            if (++dpw >= delayBufferLength) dpw = 0;

            // Store the output sample in the buffer, replacing the input
            channelData[i] = out;
        }
    }

    // Having made a local copy of the state variables for each channel, now transfer the result
    // back to the main state variable so they will be preserved for the next call of processBlock()
    delayReadPosition = dpr;
    delayWritePosition = dpw;

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = numInputChannels; i < numOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
}

// Called by the host when it needs to persist the current plugin state
void DelayProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void DelayProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}