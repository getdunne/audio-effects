#include "CompressorProcessor.h"
#include "CompressorEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CompressorProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* CompressorProcessor::createEditor()
{
    return new CompressorEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
CompressorProcessor::CompressorProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier(JucePlugin_Name), CompressorParameters::createParameterLayout())
    , parameters(valueTreeState)
    , inputBuffer(1, 1)
{
}

// Destructor
CompressorProcessor::~CompressorProcessor()
{
}

// Prepare to process audio (always called at least once before processBlock)
void CompressorProcessor::prepareToPlay (double sampleRate, int maxSamplesPerBlock)
{
    samplerate = int(sampleRate);
    bufferSize = maxSamplesPerBlock;

    x_g.allocate(bufferSize, true);
    x_l.allocate(bufferSize, true);
    y_g.allocate(bufferSize, true);
    y_l.allocate(bufferSize, true);
    c.allocate(bufferSize, true);
    yL_prev = 0;
    autoTime = false;

    yL_prev = 0;
    for (int i = 0; i < bufferSize; ++i)
    {
        x_g[i] = y_g[i] = 0.0f;
        x_l[i] = y_l[i] = 0.0f;
        c[i] = 0.0f;
    }
}

// Audio processing finished; release any allocated memory
void CompressorProcessor::releaseResources()
{
    x_g.free();
    x_l.free();
    y_g.free();
    y_l.free();
    c.free();
}

// Process one buffer ("block") of data
void CompressorProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    int M = getTotalNumInputChannels() / 2;
    inputBuffer.setSize(M, bufferSize);

    for (int m = 0; m < M; ++m)
    {
        if ((parameters.threshold < 0.0f))
        {
            // Mix down left-right to analyse the input (feedforward)
            inputBuffer.clear(m, 0, bufferSize);
            inputBuffer.addFrom(m, 0, buffer, m * 2, 0, bufferSize, 0.5f);
            inputBuffer.addFrom(m, 0, buffer, m * 2 + 1, 0, bufferSize, 0.5f);

            // compression : calculates the control voltage
            computeCompressionGain(inputBuffer, m);

            // apply control voltage to the audio signal
            const float *pIL = buffer.getReadPointer(2 * m + 0);
            const float *pIR = buffer.getReadPointer(2 * m + 1);
            float* pOL = buffer.getWritePointer(2 * m + 0);
            float* pOR = buffer.getWritePointer(2 * m + 1);
            for (int i = 0; i < bufferSize; ++i)
            {
                float cv = c[i];
                *pOL++ = cv * *pIL++;
                *pOR++ = cv * *pIR++;
            }

            // Mix down left-right to analyse the output (feedback)
            //inputBuffer.clear(m, 0, bufferSize);
            //inputBuffer.addFrom(m, 0, buffer, m * 2, 0, bufferSize, 0.5f);
            //inputBuffer.addFrom(m, 0, buffer, m * 2 + 1, 0, bufferSize, 0.5f);
        }
    }
}

void CompressorProcessor::computeCompressionGain(AudioSampleBuffer &buffer, int m)
{
    float alphaAttack = exp(-1.0f / (0.001f * samplerate * parameters.attackTimeMs));
    float alphaRelease = exp(-1.0f / (0.001f * samplerate * parameters.releaseTimeMs));

    float yl_avg = 0.0f;

    for (int i = 0; i < bufferSize; ++i)
    {
        // Level detection- estimate level using peak detector
        if (fabs(buffer.getWritePointer(m)[i]) < 0.000001f) x_g[i] = -120;
        else x_g[i] = 20 * log10(fabs(buffer.getWritePointer(m)[i]));

        // Gain computer- static apply input/output curve
        if (x_g[i] >= parameters.threshold)
            y_g[i] = parameters.threshold + (x_g[i] - parameters.threshold) / parameters.compressionRatio;
        else
            y_g[i] = x_g[i];
        x_l[i] = x_g[i] - y_g[i];

        // Ballistics- smoothing of the gain 
        if (x_l[i] > yL_prev)
            y_l[i] = alphaAttack * yL_prev + (1 - alphaAttack) * x_l[i];
        else
            y_l[i] = alphaRelease * yL_prev + (1 - alphaRelease) * x_l[i];

        // Accumulate averaged gain for the whole buffer (for GUI display)
        yl_avg += y_l[i];

        // find control
        c[i] = pow(10.0f, (parameters.makeUpGain - y_l[i]) / 20.0f);
        yL_prev = y_l[i];
    }

    yl_avg /= bufferSize;
    currentGain = pow(10.0f, -yl_avg / 20.0f);
    sendChangeMessage();
}

// Called by the host when it needs to persist the current plugin state
void CompressorProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void CompressorProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}
