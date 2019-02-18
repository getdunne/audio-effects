/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Reverb: algorithmic reverb effect based on MVerb
  See textbook Chapter 11: Reverberation
 
  Original code by Martin Eastwood: MVerb (see MVerb.h)
  Adapted for JUCE by Brecht De Man
 
  When using this code (or a modified version thereof) please cite:

  R. Stables, S. Enderby, B. De Man, G. Fazekas, J. D. Reiss, "SAFE:
  A System for the Extraction and Retrieval of Semantic Audio
  Descriptors," 15th International Society for Music Information
  Retrieval Conference (ISMIR 2014), 2014.
 
  ---

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"

ReverbAudioProcessor::ReverbAudioProcessor()
:
    _numChannels            (1)
    ,_numSamples			(1)  // dummy - will be set in prepareToPlay
    ,_sampleRate			(1)  // dummy - will be set in prepareToPlay
    ,_density               (1.0)
    ,_decay                 (1.0)
    ,_size                  (1.0)
    ,_damp                  (1.0)
    ,_bandwidth             (1.0)
    ,_predelay              (0.0)
    ,_gain                  (1.0)
    ,_mix                   (0.5)
    ,_lateEarly             (0.5)
    ,tempInput              (1,1)// dummy - will be set in prepareToPlay
    ,tempOutput             (1,1)// dummy - will be set in prepareToPlay
    ,_lastUIWidth			(850)
    ,_lastUIHeight			(650)
    
{
    // Update all parameters
    for (int index = 0; index < MVerb<float>::NUM_PARAMS; ++index)
    {
        updateParameters(index);
    }
}

ReverbAudioProcessor::~ReverbAudioProcessor()
{
}

//-----------------------------------------------------------------------------
// P R E P A R E   T O   P L A Y
void ReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // If sample rate/block size changes or straight after construction
    if (_numSamples != samplesPerBlock || _sampleRate != sampleRate)
    {
        _sampleRate  = sampleRate;
        _numSamples  = samplesPerBlock;
        _numChannels = getNumInputChannels();
        
        tempInput.setSize (_numChannels,_numSamples);
        tempOutput.setSize(_numChannels,_numSamples);
        tempInput.clear();
        tempOutput.clear();
        
        _mverb.reset();
        _mverb.setSampleRate (_sampleRate);  // set reverb sample rate
    }
}


//-----------------------------------------------------------------------------
// P R O C E S S   B L O C K
void ReverbAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
    
    for(int channel = 0; channel < numChannels; channel++)
    {
        tempInput.copyFrom (channel, 0, buffer, channel, 0, numSamples);
    }
    
    float** input = tempInput.getArrayOfWritePointers();
    float** output = tempOutput.getArrayOfWritePointers();
    
    _mverb.process (input, output, numSamples); // processing by MVerb
    
    for(int channel = 0; channel < numChannels; channel++)
    {
        buffer.copyFrom (channel, 0, output [channel], numSamples);
    }
}


//-----------------------------------------------------------------------------
// U P D A T E   P A R A M E T E R S
void ReverbAudioProcessor::updateParameters (int index)
{
    switch(index)
    {
        case MVerb<float>::DAMPINGFREQ:
            _mverb.setParameter (index, _damp);
            break;
            
        case MVerb<float>::DENSITY:
            _mverb.setParameter (index, _density);
            break;
            
        case MVerb<float>::BANDWIDTHFREQ:
            _mverb.setParameter (index, _bandwidth);
            break;
            
        case MVerb<float>::PREDELAY:
            _mverb.setParameter (index, _predelay);
            break;
            
        case MVerb<float>::DECAY:
            _mverb.setParameter (index, _decay);
            break;
            
        case MVerb<float>::SIZE:
            _mverb.setParameter (index, _size);
            break;
            
        case MVerb<float>::GAIN:
            _mverb.setParameter (index, _gain);
            break;
            
        case MVerb<float>::MIX:
            _mverb.setParameter (index, _mix);
            break;
            
        case MVerb<float>::EARLYMIX:
            _mverb.setParameter (index, _lateEarly);
            break;
    }
}


//-----------------------------------------------------------------------------
// R E S E T
void ReverbAudioProcessor::Reset()
{
    _mverb.reset(); // not used
}


//-----------------------------------------------------------------------------
//
void ReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this to free up any spare memory, etc.
}


bool ReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ReverbAudioProcessor::createEditor()
{
    return new ReverbAudioProcessorEditor (this);
}


//==============================================================================
void ReverbAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // SAVE STATE INFO
    XmlElement xml("JRAMReverb_XML");
    
    // Knobs
    xml.setAttribute("_density"     ,_density);
    xml.setAttribute("_decay"       ,_decay);
    xml.setAttribute("_size"        ,_size);
    xml.setAttribute("_damp"        ,_damp);
    xml.setAttribute("_bandwidth"   ,_bandwidth);
    xml.setAttribute("_predelay"    ,_predelay);
    xml.setAttribute("_gain"        ,_gain);
    xml.setAttribute("_mix"         ,_mix);
    xml.setAttribute("_lateEarly"   ,_lateEarly);
    
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}

void ReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // LOAD STATE INFO
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    // make sure that it's actually our type of XML object..
    if(xmlState->hasTagName("JRAMReverb_XML"))
    {
        // Knobs
        _density        = (float) xmlState->getDoubleAttribute("_density",true);
        _decay          = (float) xmlState->getDoubleAttribute("_decay",true);
        _size           = (float) xmlState->getDoubleAttribute("_size",true);
        _damp           = (float) xmlState->getDoubleAttribute("_damp",true);
        _bandwidth      = (float) xmlState->getDoubleAttribute("_bandwidth",true);
        _predelay       = (float) xmlState->getDoubleAttribute("_predelay",true);
        _gain           = (float) xmlState->getDoubleAttribute("_gain",true);
        _mix            = (float) xmlState->getDoubleAttribute("_mix",true);
        _lateEarly      = (float) xmlState->getDoubleAttribute("_lateEarly",true);
    }
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverbAudioProcessor();
}

const String ReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReverbAudioProcessor::silenceInProducesSilenceOut() const
{
    return true;
}

int ReverbAudioProcessor::getNumParameters()
{
    return MVerb<float>::NUM_PARAMS;
}

float ReverbAudioProcessor::getParameter (int index) // externally accessible
{
    switch (index)
    {
        case MVerb<float>::DENSITY:         return GetDensity();
        case MVerb<float>::DECAY:           return GetDecay();
        case MVerb<float>::SIZE:            return GetSize();
        case MVerb<float>::DAMPINGFREQ:     return GetDamp();
        case MVerb<float>::BANDWIDTHFREQ:   return GetBandwidth();
        case MVerb<float>::PREDELAY:        return GetPredelay();
        case MVerb<float>::GAIN:            return GetGain();
        case MVerb<float>::MIX:             return GetMix();
        case MVerb<float>::EARLYMIX:        return GetLateEarly();
		default:                            return 0.0f;
    }
}

void ReverbAudioProcessor::setParameter (int index, float newValue) // externally accessible
{
    switch (index)
    {
        case MVerb<float>::DENSITY:
            SetDensity(newValue);
            break;
        case MVerb<float>::DECAY:
            SetDecay(newValue);
            break;
        case MVerb<float>::SIZE:
            SetSize(newValue);
            break;
        case MVerb<float>::DAMPINGFREQ:
            SetDamp(newValue);
            break;
        case MVerb<float>::BANDWIDTHFREQ:
            SetBandwidth(newValue);
            break;
        case MVerb<float>::PREDELAY:
            SetPredelay(newValue);
            break;
        case MVerb<float>::GAIN:
            SetGain(newValue);
            break;
        case MVerb<float>::MIX:
            SetMix(newValue);
            break;
        case MVerb<float>::EARLYMIX:
            SetLateEarly(newValue);
            break;
        default:
            break;
    }
}

const String ReverbAudioProcessor::getParameterName (int index) // externally accessible
{
    switch (index)
    {
        case MVerb<float>::DENSITY:         return "Density";
        case MVerb<float>::DECAY:           return "Decay";
        case MVerb<float>::SIZE:            return "Size";
        case MVerb<float>::DAMPINGFREQ:     return "Damp";
        case MVerb<float>::BANDWIDTHFREQ:   return "Bandwidth";
        case MVerb<float>::PREDELAY:        return "Predelay";
        case MVerb<float>::GAIN:            return "Gain";
        case MVerb<float>::MIX:             return "Mix";
        case MVerb<float>::EARLYMIX:        return "Late vs. early reflections";
		default:                            break;
    }
    return String::empty;
}

const String ReverbAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String ReverbAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String ReverbAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool ReverbAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool ReverbAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool ReverbAudioProcessor::acceptsMidi() const
{
    #if JucePlugin_WantsMidiInput
        return true;
    #else
        return false;
    #endif
}

bool ReverbAudioProcessor::producesMidi() const
{
    #if JucePlugin_ProducesMidiOutput
        return true;
    #else
        return false;
    #endif
}

int ReverbAudioProcessor::getNumPrograms()
{
    return 0;
}

int ReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ReverbAudioProcessor::setCurrentProgram (int index)
{
}

const String ReverbAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void ReverbAudioProcessor::changeProgramName (int index, const String& newName)
{
}
