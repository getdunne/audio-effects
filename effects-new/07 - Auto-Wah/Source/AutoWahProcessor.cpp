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
    , parameters(valueTreeState, this)
    , envelopes(nullptr), numEnvelopes(0)
    , lfoPhase(0.0f)
    , attackMultiplier(1.0f), decayMultiplier(0.0f)
    , inverseSampleRate(1.0 / 44100.0)  // sensible default
{
    // force initializion of attackMultiplier, decayMultiplier
    parameterChanged("", 0.0f);
}

// Destructor
AutoWahProcessor::~AutoWahProcessor()
{
}

void AutoWahProcessor::parameterChanged(const String&, float)
{
    const double ONE_OVER_E = 1.0 / 2.71828182845904523536;

    if (parameters.attackTimeSec == 0.0f)
        attackMultiplier = 0.0;
    else
        attackMultiplier = pow(ONE_OVER_E, inverseSampleRate / parameters.attackTimeSec);

    if (parameters.decayTimeSec == 0.0f)
        decayMultiplier = 0.0;
    else
        decayMultiplier = pow(ONE_OVER_E, inverseSampleRate / parameters.decayTimeSec);
}

// Prepare to process audio (always called at least once before processBlock)
void AutoWahProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    inverseSampleRate = 1.0 / sampleRate;

    // create as many identical filters as there are input channels
    for (int i = 0; i < getTotalNumInputChannels(); i++)
        filters.add(new ResonantLowpassFilter);

    // same for envelopes, setting them all to zero
    if (envelopes) delete[] envelopes;
    numEnvelopes = getTotalNumInputChannels();
    envelopes = new double[numEnvelopes];
    for (int i = 0; i < numEnvelopes; i++) envelopes[i] = 0.0;
}

// Audio processing finished; release any allocated memory
void AutoWahProcessor::releaseResources()
{
    delete[] envelopes;
    envelopes = nullptr;
    numEnvelopes = 0;
    filters.clear();
}

// Process one buffer ("block") of data
void AutoWahProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    int numInputChannels = getTotalNumInputChannels();
    int numFilters = filters.size();    // should be the same
    int numOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();

    const float TWOPI_F = 6.283185f;
    const double kWahwahFilterGain = 0.5;
    float phi = lfoPhase;
    float dphi = float(parameters.lfoFreqHz * inverseSampleRate);

    int ch = 0;
    for (; ch < jmin(numInputChannels, numFilters); ch++)
    {
        // channelData is an array of length numSamples which contains the audio for one channel
        float* channelData = buffer.getWritePointer(ch);
        phi = lfoPhase;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float in = channelData[sample];
            float centreFrequency = parameters.centreFreqHz;

            // Calculate the envelope of the signal. Do this even if we're not currently
            // changing the frequency based on it, since it involves maintaining a history
            // of the signal's behaviour.
            if (ch < numEnvelopes)   // Safety check
            {
                if (fabs(in) > envelopes[ch])
                    envelopes[ch] += (1.0 - attackMultiplier) * (fabs(in) - envelopes[ch]);
                else
                    envelopes[ch] *= decayMultiplier;
            }

            // Calculate the centre frequency of the filter based on the LFO and the
            // signal envelope
            if (parameters.lfoWidthHz > 0.0f)
                centreFrequency += parameters.lfoWidthHz * (0.5f + 0.5f * sinf(TWOPI_F * phi));
            if (parameters.envWidthHz > 0.0f && ch < numEnvelopes)
                centreFrequency += parameters.envWidthHz * float(envelopes[ch]);

            // Update filter coefficients (see ResonantLowpassFilter.cpp for calculation)
            filters[ch]->makeResonantLowpass(inverseSampleRate, centreFrequency,
                parameters.filterQ, kWahwahFilterGain);

            // Process one sample and store it back in place. See juce_IIRFilter.cpp for the
            // application of the IIR filter.
            channelData[sample] = filters[ch]->processSingleSampleRaw(in);

            // Update the LFO phase, keeping it in the range 0-1
            phi += dphi;
            while (phi > 1.0f) phi -= 1.0f;
        }
    }

    lfoPhase = phi;

    // Go through the remaining channels. In case we have more outputs
    // than inputs, or there aren't enough filters, we'll clear any
    // remaining output channels (which could otherwise contain garbage)
    while (ch < numOutputChannels)
    {
        buffer.clear(ch++, 0, numSamples);
    }
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