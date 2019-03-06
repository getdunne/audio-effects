#include "VibratoProcessor.h"
#include "VibratoEditor.h"
#include "VibratoLFO.h"
#include "VibratoInterpolation.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VibratoProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* VibratoProcessor::createEditor()
{
    // Create editor
    VibratoEditor* editor = new VibratoEditor(*this);

    // Force initial update of pitchShiftDescription and GUI label
    parameterChanged({}, 0.0f);

    return editor;
}

// Constructor: start off assuming stereo input, stereo output
VibratoProcessor::VibratoProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("Vibrato"), VibratoParameters::createParameterLayout())
    , parameters(valueTreeState, this)
    , lfoPhase(0.0f)
    , inverseSampleRate(1.0f / 44100.0f)
    , delayBufferLength(0)
    , delayReadPosition(0), delayWritePosition(0)
{
}

// Destructor
VibratoProcessor::~VibratoProcessor()
{
}

// Called whenever LFO waveform, LFO frequency, or sweep width parameters are changed.
void VibratoProcessor::parameterChanged(const String&, float)
{
    // The amount of pitch shift depends on the derivative of the delay, which
    // is given by: delay = width * f(frequency * t)
    // where f(x) is one of:
    //   sine --> 0.5 + 0.5*sin(2*pi*x) --> derivative pi*cos(x)*dx
    //   triangle --> {2.0*x or 1.0-(2.0*(x-0.5)) ---> derivative +/- 2.0*dx
    //   sawtooth rising --> x --> derivative 1.0*dx
    //   sawtooth falling --> 1.0 - x --> derivative -1.0*dx
    // For f(frequency*t), "dx" = frequency

    float maxSpeed = 1.0, minSpeed = 1.0;
    float maxPitch = 0.0, minPitch = 0.0;

    float lfoFreqTimesSweep = parameters.lfoFreqHz * parameters.sweepWidthHz;
    const float FLT_PI = 3.1415926f;
    switch (parameters.lfoWaveform)
    {
    case VibratoLFO::kWaveformSine:
        maxSpeed = 1.0f + FLT_PI * lfoFreqTimesSweep;
        minSpeed = 1.0f - FLT_PI * lfoFreqTimesSweep;
        break;
    case VibratoLFO::kWaveformTriangle:
        maxSpeed = 1.0f + 2.0f * lfoFreqTimesSweep;
        minSpeed = 1.0f - 2.0f * lfoFreqTimesSweep;
        break;
    case VibratoLFO::kWaveformSawtooth:
        // Standard (rising) sawtooth means delay is increasing --> pitch is lower
        maxSpeed = 1.0f;
        minSpeed = 1.0f - lfoFreqTimesSweep;
        break;
    case VibratoLFO::kWaveformInverseSawtooth:
        // Inverse (falling) sawtooth means delay is decreasing --> pitch is higher
        maxSpeed = 1.0f + lfoFreqTimesSweep;
        minSpeed = 1.0f;
        break;
    }

    // Convert speed to pitch shift --> semitones = 12*log2(speed)
    maxPitch = 12.0f * logf(maxSpeed) / logf(2.0f);

    if (minSpeed > 0)
    {
        minPitch = 12.0f* logf(minSpeed) / logf(2.0f);
        pitchShiftDescription = String::formatted("Vibrato range: %+.2f to %+.2f semitones (speed %.3f to %.3f)",
                                                  minPitch, maxPitch, minSpeed, maxSpeed);
    }
    else
    {
        pitchShiftDescription = String::formatted("Vibrato range: 0.0 to %+.2f semitones (speed %.3f to %.3f)",
                                                  maxPitch, minSpeed, maxSpeed);
    }

    // Signal any active editor to update its pitchShiftDescriptionLabel
    sendChangeMessage();
}

// Prepare to process audio (always called at least once before processBlock)
void VibratoProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    // Allocate and zero the delay buffer (size will depend on current sample rate)
    // Add 3 extra samples to allow cubic interpolation even at maximum delay
    delayBufferLength = (int)(VibratoParameters::sweepWidthMax * sampleRate) + 3;
    delayBuffer.setSize(2, delayBufferLength);
    delayBuffer.clear();
    lfoPhase = 0.0f;

    inverseSampleRate = 1.0 / sampleRate;
}

// Audio processing finished; release any allocated memory
void VibratoProcessor::releaseResources()
{
}

// Process one buffer ("block") of data
void VibratoProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
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
            float interpolatedSample = 0.0;

            // Recalculate the read pointer position with respect to the write pointer. A more efficient
            // implementation might increment the read pointer based on the derivative of the LFO without
            // running the whole equation again, but this format makes the operation clearer.
            currentDelay = parameters.sweepWidthHz * VibratoLFO::getSample(ph, parameters.lfoWaveform);

            // Subtract 3 samples to the delay pointer to make sure we have enough previously written
            // samples to interpolate with
            dpr = fmodf(float(dpw - (currentDelay * getSampleRate()) + delayBufferLength - 3.0),
                        float(delayBufferLength));

            // In this example, the output is the input plus the contents of the delay buffer (weighted by delayMix)
            // The last term implements a tremolo (variable amplitude) on the whole thing.
            interpolatedSample = VibratoInterpolation::processSample(parameters.interpolationType,
                                                                     dpr, delayData, delayBufferLength);

            // Store the current information in the delay buffer. With feedback, what we read is
            // included in what gets stored in the buffer, otherwise it's just a simple delay line
            // of the input signal.
            delayData[dpw] = in;

            // Increment the write pointer at a constant rate. The read pointer will move at different
            // rates depending on the settings of the LFO, the delay and the sweep width.
            if (++dpw >= delayBufferLength) dpw = 0;

            // Store the output sample in the buffer, replacing the input. In the vibrato effect,
            // the delaye sample is the only component of the output (no mixing with the dry signal)
            channelData[i] = interpolatedSample;

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
void VibratoProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void VibratoProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}