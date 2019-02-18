/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Distortion: distortion effect using different characteristic curves
  See textbook Chapter 7: Overdrive, Distortion and Fuzz
 
  Code by Brecht De Man, Joshua Reiss and Andrew McPherson
 
  When using this code (or a modified version thereof), please cite:
 
  Brecht De Man and Joshua D. Reiss, "Adaptive Control of Amplitude
  Distortion Effects," 53rd Conference of the Audio Engineering Society,
  2014.
 
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

#if JUCE_INTEL
 #define JUCE_SNAP_TO_ZERO(n)    if (! (n < -1.0e-8 || n > 1.0e-8)) n = 0;
#else
 #define JUCE_SNAP_TO_ZERO(n)
#endif

DistortionAudioProcessor::DistortionAudioProcessor()
:
    _numChannels            (1)
    ,_numSamples			(1)  // dummy - will be set in prepareToPlay
    ,_sampleRate			(1)  // dummy - will be set in prepareToPlay
    ,_typeNumber            (_hardClipping) // standard
    ,_currentTrackBuffer	(1,1)
    ,_lastUIWidth			(850)
    ,_lastUIHeight			(650)
    
{
    Reset();
}

DistortionAudioProcessor::~DistortionAudioProcessor()
{
}

//-----------------------------------------------------------------------------
// P R E P A R E   T O   P L A Y
void DistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // If sample rate/block size changes or straight after construction
    if (_numSamples != samplesPerBlock || _sampleRate != sampleRate)
    {
        _sampleRate  = sampleRate;
        _numSamples  = samplesPerBlock;
        _numChannels = getNumInputChannels();
    }
}


//-----------------------------------------------------------------------------
// P R O C E S S   B L O C K
void DistortionAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    float gain = powf(10.0f, _gainIndB/20.f);
    
    
    for (int channel = 0; channel < _numChannels; ++channel)
    {
        // Apply gain
        buffer.applyGain(channel, 0, buffer.getNumSamples(), gain);
        
        // Put track audio data into _currentTrackBuffer
        float * originalData = new float;
        originalData = buffer.getWritePointer(channel);
        
        // Apply distortion (sample per sample)
        switch (_typeNumber) {
            case _hardClipping:
            {
                float threshold = 0.5f;
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    if(originalData[sample]  > threshold) // positive hard clipping
                    {
                        originalData[sample] = threshold;
                    }
                    else
                    {
                        if(originalData[sample]  < - threshold) // negative hard clipping
                        {
                            originalData[sample] = - threshold;
                        }
                    }
                }
                break;
            }
            
            case _softClipping:
            {
                float threshold1 = 1.0f/3.0f;
                float threshold2 = 2.0f/3.0f;
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    if(originalData[sample]  > threshold1)
                    {
                        if(originalData[sample] > threshold2) // positive clipping
                        {
                            originalData[sample] = 1.0f;
                        }
                        else // soft knee (positive)
                        {
                            originalData[sample] = (3.0f - (2.0f - 3.0f*originalData[sample])*(2.0f - 3.0f*originalData[sample]))/3.0f;
                        }
                    }
                    else
                    {
                        if(originalData[sample]  < -threshold1)
                        {
                            if(originalData[sample] < -threshold2) // negative clipping
                            {
                                originalData[sample] = -1.0f;
                            }
                            else // soft knee (negative)
                            {
                                originalData[sample] = - (3.0f - (2.0f + 3.0f*originalData[sample])*(2.0f + 3.0f*originalData[sample]))/3.0f;
                            }
                        }
                        else // linear region (-1/3..1/3)
                        {
                            originalData[sample] *= 2.0f;
                        }
                    }
                    
                    originalData[sample] /= 2.0f; // divide all by 2 to compensate for extra 6 dB gain boost
                }
                break;
            }
                
            case _softClippingExp:
            {
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    if (originalData[sample] > 0.0f) // positive
                    {
                        originalData[sample] = 1.0f - expf(-originalData[sample]);
                    }
                    else // negative
                    {
                        originalData[sample] = - 1.0f + expf(originalData[sample]);
                    }
                }
                break;
            }
                
            case _fullWaveRectifier:
            {
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    originalData[sample] = fabs(originalData[sample]);
                }
                break;
            }
                
            case _halfWaveRectifier:
            {
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    originalData[sample] = 0.5f*(fabs(originalData[sample])+originalData[sample]);
                }
                break;
            }
                
            default:
                break;
        }
    }
}


//-----------------------------------------------------------------------------
// R E S E T
void DistortionAudioProcessor::Reset()
{
    _gainIndB   = 0.0f;
    _typeNumber = _hardClipping;
}


//-----------------------------------------------------------------------------
//
void DistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this to free up any spare memory, etc.
}


bool DistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DistortionAudioProcessor::createEditor()
{
    return new DistortionAudioProcessorEditor (this);
}


//==============================================================================
void DistortionAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // SAVE STATE INFO
    XmlElement xml("Distortion_XML");
    
    // Knobs
    xml.setAttribute("_gain"       ,_gainIndB);
    
    // Combo box
    xml.setAttribute("_type"       ,(int) _typeNumber);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}

void DistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // LOAD STATE INFO
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    // make sure that it's actually our type of XML object..
    if(xmlState->hasTagName("Distortion_XML"))
    {
        
        // Knobs
        _gainIndB       = (float) xmlState->getDoubleAttribute("_gain",true);
        
        //Combo box
        _typeNumber     = (DistortionAudioProcessor::Types) xmlState->getIntAttribute("_type",true);
    }
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionAudioProcessor();
}

const String DistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistortionAudioProcessor::silenceInProducesSilenceOut() const
{
    return true;
}

int DistortionAudioProcessor::getNumParameters()
{
    return _PARAMtotalNumParams;
}

float DistortionAudioProcessor::getParameter (int index) // externally accessible
{
    switch (index)
    {
        case _PARAMdeviceReset:     return 0.0f;
        case _PARAMgain:            return (GetGainIndB());
        case _PARAMtype:            return (GetType());
		default:					return 0.0f;
    }
}

void DistortionAudioProcessor::setParameter (int index, float newValue) // externally accessible
{
    switch (index)
    {
        case _PARAMdeviceReset:
            Reset();
            break;
        case _PARAMgain:
            SetGainIndB(newValue);
            break;
        case _PARAMtype:
            SetType((DistortionAudioProcessor::Types) roundFloatToInt(newValue*_numberOfTypes));
            break;
        default:
            break;
    }
}

const String DistortionAudioProcessor::getParameterName (int index) // externally accessible
{
    switch (index)
    {
        case _PARAMdeviceReset:     return "Reset";
        case _PARAMgain:            return "Gain (dB)";
        case _PARAMtype:            return "Type";
		default:					break;
    }
    return String::empty;
}

const String DistortionAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String DistortionAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String DistortionAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool DistortionAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool DistortionAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool DistortionAudioProcessor::acceptsMidi() const
{
    #if JucePlugin_WantsMidiInput
        return true;
    #else
        return false;
    #endif
}

bool DistortionAudioProcessor::producesMidi() const
{
    #if JucePlugin_ProducesMidiOutput
        return true;
    #else
        return false;
    #endif
}

int DistortionAudioProcessor::getNumPrograms()
{
    return 0;
}

int DistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistortionAudioProcessor::setCurrentProgram (int index)
{
}

const String DistortionAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void DistortionAudioProcessor::changeProgramName (int index, const String& newName)
{
}
