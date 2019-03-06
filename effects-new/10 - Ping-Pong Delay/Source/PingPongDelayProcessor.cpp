#include "PingPongDelayProcessor.h"
#include "PingPongDelayEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PingPongDelayProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* PingPongDelayProcessor::createEditor()
{
    return new PingPongDelayEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
PingPongDelayProcessor::PingPongDelayProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("PingPongDelay"), PingPongDelayParameters::createParameterLayout())
    , parameters(valueTreeState, this)
    , delayReadPositionLeft(0), delayReadPositionRight(0)
    , delayWritePosition(0)
{
}

// Destructor
PingPongDelayProcessor::~PingPongDelayProcessor()
{
}

void PingPongDelayProcessor::parameterChanged(const String&, float)
{
    delayReadPositionLeft = (int)(delayWritePosition - (parameters.delaySecL * getSampleRate())
                                                     + delayBufferLength) % delayBufferLength;

    delayReadPositionRight = (int)(delayWritePosition - (parameters.delaySecR * getSampleRate())
                                                      + delayBufferLength) % delayBufferLength;
}

// Prepare to process audio (always called at least once before processBlock)
void PingPongDelayProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
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
    delayReadPositionLeft = (int)(delayWritePosition - (parameters.delaySecL * sampleRate)
                                                     + delayBufferLength) % delayBufferLength;
    delayReadPositionRight = (int)(delayWritePosition - (parameters.delaySecR * sampleRate)
                                                      + delayBufferLength) % delayBufferLength;
}

// Audio processing finished; release any allocated memory
void PingPongDelayProcessor::releaseResources()
{
}

// Process one buffer ("block") of data
void PingPongDelayProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    // Helpful information about this block of samples:
    const int numInputChannels = getTotalNumInputChannels();    // How many input channels for our effect?
    const int numOutputChannels = getTotalNumOutputChannels();  // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();              // How many samples in the buffer for this block?

    // This shouldn't happen, but we need a sanity check: this effect only makes sense
    // if there are at least 2 channels to work with (and in this case only 2...)
    if (buffer.getNumChannels() < 2)
        return;

    // If there is one input only, the second channel may not contain anything useful.
    // start with a blank buffer in this case
    if (numInputChannels < 2)
        buffer.clear(1, 0, numSamples);

    // channelDataL and channelDataR are arrays of length numSamples which contain
    // the audio for one channel
    float *channelDataL = buffer.getWritePointer(0);
    float *channelDataR = buffer.getWritePointer(1);

    // delayDataL and delayDataR are the circular buffers for implementing delay
    float* delayDataL = delayBuffer.getWritePointer(0);
    float* delayDataR = delayBuffer.getWritePointer(1);

    for (int i = 0; i < numSamples; ++i)
    {
        const float inL = channelDataL[i];
        const float inR = channelDataR[i];
        float outL, outR;

        if (parameters.reverseChannels)
        {
            outL = (inL + parameters.wetLevel * delayDataR[delayReadPositionLeft]);
            outR = (inR + parameters.wetLevel * delayDataL[delayReadPositionRight]);
        }
        else
        {
            outL = (inL + parameters.wetLevel * delayDataL[delayReadPositionLeft]);
            outR = (inR + parameters.wetLevel * delayDataR[delayReadPositionRight]);
        }

        // Store the output of one delay buffer into the other, producing
        // the ping-pong effect
        delayDataR[delayWritePosition] = inR + (delayDataL[delayReadPositionLeft] * parameters.feedback);
        delayDataL[delayWritePosition] = inL + (delayDataR[delayReadPositionRight] * parameters.feedback);

        if (++delayReadPositionLeft >= delayBufferLength) delayReadPositionLeft = 0;
        if (++delayReadPositionRight >= delayBufferLength) delayReadPositionRight = 0;
        if (++delayWritePosition >= delayBufferLength) delayWritePosition = 0;

        // Store the output samples in the buffer, replacing the input
        channelDataL[i] = outL;
        channelDataR[i] = outR;
    }

    // Clear any channels above 2 (stereo)
    for (int i = 2; i < numOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
}

// Called by the host when it needs to persist the current plugin state
void PingPongDelayProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(new XmlElement("PingPongDelay"));
    
    // Save two parameters which are not included in the valueTreeState
    xml->setAttribute("linkDelays", parameters.linkDelays);
    xml->setAttribute("reverseChannels", parameters.reverseChannels);

    // save valueTreeState as a subtree
    xml->addChildElement(valueTreeState.state.createXml());

    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void PingPongDelayProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName("PingPongDelay"))
    {
        parameters.linkDelays = xml->getBoolAttribute("linkDelays", false);
        parameters.reverseChannels = xml->getBoolAttribute("reverseChannels", false);

        XmlElement* vtsXml = xml->getChildByName(valueTreeState.state.getType());
        if (vtsXml) valueTreeState.state = ValueTree::fromXml(*xml);
    }
}
