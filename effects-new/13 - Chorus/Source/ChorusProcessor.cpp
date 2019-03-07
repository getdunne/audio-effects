#include "ChorusProcessor.h"
#include "ChorusEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChorusProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* ChorusProcessor::createEditor()
{
    return new ChorusEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
ChorusProcessor::ChorusProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("Chorus"), ChorusParameters::createParameterLayout())
    , parameters(valueTreeState)
    , lfoPhase(0.0f)
    , inverseSampleRate(1.0f / 44100.0f)
    , delayBufferLength(0)
    , delayWritePosition(0)
{
}

// Destructor
ChorusProcessor::~ChorusProcessor()
{
}

// Prepare to process audio (always called at least once before processBlock)
void ChorusProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    // Allocate and zero the delay buffer (size will depend on current sample rate)
    // Add 3 extra samples to allow cubic interpolation even at maximum delay
    double maxDelaySec = 0.001 * ChorusParameters::minDelayMax + ChorusParameters::sweepWidthMax;
    delayBufferLength = (int)(maxDelaySec * sampleRate) + 3;
    delayBuffer.setSize(2, delayBufferLength);
    delayBuffer.clear();
    lfoPhase = 0.0f;

    inverseSampleRate = 1.0 / sampleRate;
}

// Audio processing finished; release any allocated memory
void ChorusProcessor::releaseResources()
{
}

// Process one buffer ("block") of data
void ChorusProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
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

        for (int i = 0; i < numSamples; ++i)
        {
            const float in = channelData[i];
            float interpolatedSample = 0.0f;
            float phaseOffset = 0.0f;
            float weight;

            // Chorus can have more than 2 voices (where the original, undelayed signal counts as a voice).
            // In this implementation, all voices use the same LFO, but with different phase offsets. It
            // is also possible to use different waveforms and different frequencies for each voice.
            for (int j = 0; j < parameters.voiceCount - 1; ++j)
            {
                if (parameters.stereoMode && (parameters.voiceCount > 2))
                {
                    // A stereo chorus pans each voice to a different location in the stereo field.
                    // How this is done depends on the number of voices:
                    // -- 2 voices: N/A (need at least 2 delayed voices for stereo chorus)
                    // -- 3 voices: 1 voice left, 1 voice right (0, 1)
                    // -- 4 voices: 1 voice left, 1 voice centre, 1 voice right (0, 0.5, 1)
                    // -- 5 voices: 1 voice left, 1 voice left-centre,
                    //              1 voice right-centre, 1 voice right (0, 0.33, 0.66, 1)
                    weight = float(j) / float(parameters.voiceCount - 2);

                    // Left and right channels are mirrors of each other in weight
                    if ((channel % 2) != 0) weight = 1.0f - weight;
                }
                else
                    weight = 1.0f;

                // Add the voice to the mix if it has nonzero weight
                if (weight != 0.0f)
                {
                    float lfoSample = ChorusLFO::getSample(fmodf(ph + phaseOffset, 1.0f), parameters.lfoWaveform);
                    currentDelay = 0.001f * (parameters.minDelayMs + lfoSample * parameters.sweepWidthMs);
                    dpr = fmodf(float(dpw) - float(currentDelay * getSampleRate()) + float(delayBufferLength),
                        float(delayBufferLength));

                    // In this example, the output is the input plus the contents of the delay buffer (weighted by delayMix)
                    // The last term implements a tremolo (variable amplitude) on the whole thing.
                    interpolatedSample = ChorusInterpolation::processSample(parameters.interpolationType,
                                                                            dpr, delayData, delayBufferLength);

                    // Store the output sample in the buffer, which starts by containing the input sample
                    channelData[i] += parameters.depth * weight * interpolatedSample;
                }

                // 3-voice chorus uses two voices in quadrature phase (90 degrees apart). Otherwise,
                // spread the voice phases evenly around the unit circle. (For 2-voice chorus, this
                // code doesn't matter since the loop only runs once.)
                if (parameters.voiceCount < 3)
                    phaseOffset += 0.25f;
                else
                    phaseOffset += 1.0f / (float)(parameters.voiceCount - 1);
            }

            // Store the current input in the delay buffer (no feedback in a chorus, unlike a flanger).
            delayData[dpw] = in;

            // Increment the write pointer at a constant rate. The read pointer will move at different
            // rates depending on the settings of the LFO, the delay and the sweep width.
            if (++dpw >= delayBufferLength) dpw = 0;

            // Update the LFO phase, keeping it in the range 0-1
            ph += float(parameters.lfoFreqHz * inverseSampleRate);
            while (ph >= 1.0f) ph -= 1.0f;
        }
    }

    // Having made a local copy of the state variables for each channel, now transfer the result
    // back to the main state variable so they will be preserved for the next call of processBlock()
    delayWritePosition = dpw;
    lfoPhase = ph;

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = numInputChannels; i < numOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
}

// Called by the host when it needs to persist the current plugin state
void ChorusProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void ChorusProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}