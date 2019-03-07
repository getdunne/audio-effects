#include "FlangerProcessor.h"
#include "FlangerEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FlangerProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* FlangerProcessor::createEditor()
{
    return new FlangerEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
FlangerProcessor::FlangerProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("Flanger"), FlangerParameters::createParameterLayout())
    , parameters(valueTreeState)
    , lfoPhase(0.0f)
    , inverseSampleRate(1.0f / 44100.0f)
    , delayBufferLength(0)
    , delayWritePosition(0)
{
}

// Destructor
FlangerProcessor::~FlangerProcessor()
{
}

// Prepare to process audio (always called at least once before processBlock)
void FlangerProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    // Allocate and zero the delay buffer (size will depend on current sample rate)
    // Add 3 extra samples to allow cubic interpolation even at maximum delay
    double maxDelaySec = 0.001 * FlangerParameters::minDelayMax + FlangerParameters::sweepWidthMax;
    delayBufferLength = (int)(maxDelaySec * sampleRate) + 3;
    delayBuffer.setSize(2, delayBufferLength);
    delayBuffer.clear();
    lfoPhase = 0.0f;

    inverseSampleRate = 1.0 / sampleRate;
}

// Audio processing finished; release any allocated memory
void FlangerProcessor::releaseResources()
{
}

// Process one buffer ("block") of data
void FlangerProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    // Helpful information about this block of samples:
    const int numInputChannels = getTotalNumInputChannels();    // How many input channels for our effect?
    const int numOutputChannels = getTotalNumOutputChannels();  // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();              // How many samples in the buffer for this block?

    // working variables
    int dpw = delayWritePosition;   // delay write position (integer)
    float dpr;                      // delay read position (real-valued)
    float currentDelay;
    float ph = lfoPhase;
    float channel0EndPhase = lfoPhase;

    // Go through each channel of audio that's passed in. In this example we apply identical
    // effects to each channel, regardless of how many input channels there are. For some effects, like
    // a stereo chorus or panner, you might do something different for each channel.
    int channel;
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
        dpw = delayWritePosition;
        ph = lfoPhase;

        // For stereo flanging, keep the channels 90 degrees out of phase with each other
        if (parameters.stereoMode && channel != 0)
            ph = fmodf(ph + 0.25f, 1.0f);

        for (int i = 0; i < numSamples; ++i)
        {
            const float in = channelData[i];
            float interpolatedSample = 0.0;

            // Recalculate the read pointer position with respect to the write pointer. A more efficient
            // implementation might increment the read pointer based on the derivative of the LFO without
            // running the whole equation again, but this format makes the operation clearer.
            float lfoSample = FlangerLFO::getSample(ph, parameters.lfoWaveform);
            currentDelay = 0.001f * (parameters.minDelayMs + lfoSample * parameters.sweepWidthMs);
            dpr = fmodf(float(dpw) - float(currentDelay * getSampleRate()) + float(delayBufferLength),
                        float(delayBufferLength));

            // In this example, the output is the input plus the contents of the delay buffer (weighted by delayMix)
            // The last term implements a tremolo (variable amplitude) on the whole thing.
            interpolatedSample = FlangerInterpolation::processSample(parameters.interpolationType,
                                                                     dpr, delayData, delayBufferLength);
            // Store the current information in the delay buffer. With feedback, what we read is
            // included in what gets stored in the buffer, otherwise it's just a simple delay line
            // of the input signal.
            delayData[dpw] = in + (interpolatedSample * parameters.feedback);

            // Increment the write pointer at a constant rate. The read pointer will move at different
            // rates depending on the settings of the LFO, the delay and the sweep width.
            if (++dpw >= delayBufferLength) dpw = 0;

            // Store the output sample in the buffer, replacing the input
            channelData[i] = in + parameters.depth * interpolatedSample;

            // Update the LFO phase, keeping it in the range 0-1
            ph += float(parameters.lfoFreqHz * inverseSampleRate);
            while (ph >= 1.0f) ph -= 1.0f;
        }

        // Use channel 0 only to keep the phase in sync between calls to processBlock()
        // Otherwise quadrature phase on multiple channels will create problems.
        if (channel == 0)
            channel0EndPhase = ph;
    }

    // Having made a local copy of the state variables for each channel, now transfer the result
    // back to the main state variable so they will be preserved for the next call of processBlock()
    delayWritePosition = dpw;
    lfoPhase = channel0EndPhase;

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = numInputChannels; i < numOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
}

// Called by the host when it needs to persist the current plugin state
void FlangerProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void FlangerProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}