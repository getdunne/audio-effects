#include "PhaserProcessor.h"
#include "PhaserEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PhaserProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* PhaserProcessor::createEditor()
{
    return new PhaserEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
PhaserProcessor::PhaserProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", AudioChannelSet::stereo(), true)
        .withOutput("Output", AudioChannelSet::stereo(), true)
    )
    , valueTreeState(*this, nullptr, Identifier("Phaser"), PhaserParameters::createParameterLayout())
    , parameters(valueTreeState, this)
    , lfoPhase(0.0f)
    , inverseSampleRate(1.0f / 44100.0f)
    , filterUpdateInterval(8)
    , sampleCount(0)
    , lastFilterOutputs(nullptr), numLastFilterOutputs(0)
{
}

// Destructor
PhaserProcessor::~PhaserProcessor()
{
    if (lastFilterOutputs) delete[] lastFilterOutputs;
}

// Gets called any time parameters.numFilters changes
void PhaserProcessor::parameterChanged(const String&, float)
{
    allocateFilters(parameters.numFilters);
}

void PhaserProcessor::allocateFilters(int numFilters)
{
    // get a lock on the AudioProcessor CriticalSection object which is automatically locked while host
    // is calling the processBlock() method, i.e., ensure we don't do this during audio processing
    const ScopedLock myScopedLock(getCallbackLock());

    allpassFilters.clear();
    int M = getTotalNumInputChannels();
    for (int i = 0; i < numFilters * M; i++)
    {
        allpassFilters.add(new OnePoleAllpassFilter);
    }

    if (lastFilterOutputs) delete[] lastFilterOutputs;
    numLastFilterOutputs = numFilters;
    lastFilterOutputs = new float[numLastFilterOutputs];
    for (int i = 0; i < numLastFilterOutputs; i++) lastFilterOutputs[i] = 0.0f;
}

// Prepare to process audio (always called at least once before processBlock)
void PhaserProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    inverseSampleRate = 1.0 / sampleRate;
    lfoPhase = 0.0;
    sampleCount = 0;

    allocateFilters(parameters.numFilters);
}

// Audio processing finished; release any allocated memory
void PhaserProcessor::releaseResources()
{
    if (lastFilterOutputs)
    {
        delete[] lastFilterOutputs;
        lastFilterOutputs = nullptr;
        numLastFilterOutputs = 0;
    }
}

// Process one buffer ("block") of data
void PhaserProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    // Helpful information about this block of samples:
    const int numInputChannels = getTotalNumInputChannels();    // How many input channels for our effect?
    const int numOutputChannels = getTotalNumOutputChannels();  // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();              // How many samples in the buffer for this block?

    // working variables
    float ph, channel0EndPhase = lfoPhase;
    unsigned int sc = sampleCount;
    int filtersPerChannel = parameters.numFilters;

    // Go through each channel of audio that's passed in, applying one or more allpass filters
    // to each. Each channel will be treated identically in a (non-stereo) phaser, but we have
    // to have separate filter objects for each channel since the filters store the last few samples
    // passed through them.
    // Filters are stored with all channel 0 filters first, then all channel 1 filters, etc.

    for (int channel = 0; channel < numInputChannels; ++channel)
    {
        // channelData is an array of length numSamples which contains the audio for one channel
        float* channelData = buffer.getWritePointer(channel);

        ph = lfoPhase;
        sc = sampleCount;

        // For stereo phasing, keep the channels 90 degrees out of phase with each other
        if (parameters.stereoMode && channel != 0)
            ph = fmodf(ph + 0.25f, 1.0f);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float out = channelData[sample];

            // If feedback is enabled, include the feedback from the last sample in the
            // input of the allpass filter chain. This is actually not accurate to how
            // analog phasers work because there is a sample of delay between output and
            // input, which adds a further phase shift of up to 180 degrees at half the
            // sampling frequency. To truly model an analog phaser with feedback involves
            // modelling a delay-free loop, which is beyond the scope of this example.

            if (parameters.feedback != 0.0 && channel < numLastFilterOutputs)
                out += parameters.feedback * lastFilterOutputs[channel];

            // See OnePoleAllpassFilter.cpp for calculation of coefficients and application
            // of filter to audio data. The filter processes the audio buffer in place,
            // putting the output sample in place of the input.

            for (int j = 0; j < filtersPerChannel; ++j)
            {
                // Safety check
                if (channel * filtersPerChannel + j >= allpassFilters.size())
                    continue;

                // First, update the current allpass filter coefficients depending on the parameter
                // settings and the LFO phase
                auto filter = allpassFilters[channel * filtersPerChannel + j];

                // Recalculating the filter coefficients is much more expensive than calculating
                // a sample. Only update the coefficients at a fraction of the sample rate; since
                // the LFO moves slowly, the difference won't generally be audible.
                if (sc % filterUpdateInterval == 0)
                {
                    float lfoSample = PhaserLFO::getSample(ph, parameters.lfoWaveform);
                    double centreFrequency = parameters.baseFreqHz + parameters.sweepWidthHz * lfoSample;
                    filter->makeAllpass(inverseSampleRate, centreFrequency);
                }
                out = filter->processSingleSampleRaw(out);
            }

            // update last filter output
            if (channel < numLastFilterOutputs) lastFilterOutputs[channel] = out;

            // Add the allpass signal to the output, though maintaining constant level
            // depth = 0 --> input only ; depth = 1 --> evenly balanced input and output
            float dfrac = 0.5f * parameters.depth;
            channelData[sample] = (1.0f - dfrac) * channelData[sample] + dfrac * out;

            // Update the LFO phase, keeping it in the range 0-1
            ph += float(parameters.lfoFreqHz * inverseSampleRate);
            while (ph >= 1.0f) ph -= 1.0f;

            // Update sample counter
            sc++;
        }

        // Use channel 0 only to keep the phase in sync between calls to processBlock()
        // Otherwise quadrature phase on multiple channels will create problems.
        if (channel == 0) channel0EndPhase = ph;
    }

    lfoPhase = channel0EndPhase;
    sampleCount = sc;

    // Go through the remaining channels. In case we have more outputs
    // than inputs, or there aren't enough filters, we'll clear any
    // remaining output channels (which could otherwise contain garbage)
    for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
    {
        buffer.clear(channel++, 0, buffer.getNumSamples());
    }
}

// Called by the host when it needs to persist the current plugin state
void PhaserProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void PhaserProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}