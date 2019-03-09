#include "LeslieProcessor.h"
#include "LeslieEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LeslieProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* LeslieProcessor::createEditor()
{
    return new LeslieEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
LeslieProcessor::LeslieProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("Leslie"), LeslieParameters::createParameterLayout())
    , parameters(valueTreeState, this)
    , midiSustainControlsLeslieSpeed(false)
    , midiModWheelControlsLeslieSpeed(false)
{
}

// Destructor
LeslieProcessor::~LeslieProcessor()
{
}

bool LeslieProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    int ins = layouts.getMainInputChannels();
    int outs = layouts.getMainOutputChannels();

    // accept stereo in/out only
    return (ins == 2 && outs == 2);
}

void LeslieProcessor::parameterChanged(const String&, float)
{
    leslie.setSpeed(parameters.speed);
}

// Prepare to process audio (always called at least once before processBlock)
void LeslieProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    leslie.init(sampleRate);
}

// Audio processing finished; release any allocated memory
void LeslieProcessor::releaseResources()
{
    leslie.deinit();
}

// Process one buffer ("block") of data
void LeslieProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const float *inBuffers[2] = { buffer.getReadPointer(0), buffer.getReadPointer(1) };
    float* outBuffers[2] = { buffer.getWritePointer(0), buffer.getWritePointer(1) };

    MidiBuffer::Iterator it(midiMessages);
    MidiMessage msg;
    int samplePos;
    while (it.getNextEvent(msg, samplePos))
    {
        if (midiSustainControlsLeslieSpeed)
        {
            if (msg.isSustainPedalOn())
            {
                Value speed = valueTreeState.getParameterAsValue(parameters.speedID);
                speed = 8.0f;
            }
            else if (msg.isSustainPedalOff())
            {
                Value speed = valueTreeState.getParameterAsValue(parameters.speedID);
                speed = 4.0f;
            }
        }
        else if (midiModWheelControlsLeslieSpeed)
        {
            if (msg.isControllerOfType(1))
            {
                Value speed = valueTreeState.getParameterAsValue(parameters.speedID);
                // convert controller value [0, 127] to range [0, 8]
                speed = float(int(0.5f + msg.getControllerValue() / 16.0f));
            }
        }
    }
    midiMessages.clear(0, buffer.getNumSamples());

    leslie.render(buffer.getNumSamples(), inBuffers, outBuffers);
}

// Called by the host when it needs to persist the current plugin state
void LeslieProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(new XmlElement("Leslie"));

    // Save two parameters which are not included in the valueTreeState
    xml->setAttribute("midiSustainControlsLeslieSpeed", midiSustainControlsLeslieSpeed);
    xml->setAttribute("midiModWheelControlsLeslieSpeed", midiModWheelControlsLeslieSpeed);

    // save valueTreeState as a subtree
    xml->addChildElement(valueTreeState.state.createXml());

    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void LeslieProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName("Leslie"))
    {
        midiSustainControlsLeslieSpeed = xml->getBoolAttribute("midiSustainControlsLeslieSpeed", false);
        midiModWheelControlsLeslieSpeed = xml->getBoolAttribute("midiModWheelControlsLeslieSpeed", false);

        XmlElement* vtsXml = xml->getChildByName(valueTreeState.state.getType());
        if (vtsXml) valueTreeState.state = ValueTree::fromXml(*xml);
    }
}
