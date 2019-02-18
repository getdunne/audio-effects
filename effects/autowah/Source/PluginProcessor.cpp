/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Auto-Wah: LFO or envelope-operated wah effect
  See textbook Chapter 4: Filter Effects
 
  Code by Andrew McPherson, Brecht de Man and Joshua Reiss
 
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

// The filter will produce a resonant peak of amplitude Q; bring everything
// down somewhat to compensate, though try to maintain some perceptual balance
// of being similar loudness. (This factor has been chosen somewhat arbitrarily.)
const double kWahwahFilterGain = 0.5;

//==============================================================================
AutoWahAudioProcessor::AutoWahAudioProcessor()
{
    // Set default values:
    baseFrequency_ = 350.0;
    q_ = 5.0;
    lfoFrequency_ = 2.0;
    lfoWidth_ = 1000.0;
    envelopeWidth_ = 0.0;
    envelopeAttack_ = 0.005;
    envelopeDecay_ = 0.1;
    
    // Initialise the filters later when we know how many channels
    wahFilters_ = 0;
    numWahFilters_ = 0;
    envelopes_ = 0;
    numEnvelopes_ = 0;
    attackMultiplier_ = 1.0;
    decayMultiplier_ = 0.0;
    
    inverseSampleRate_ = 1.0/44100.0; // start with a sensible default
    
    lastUIWidth_ = 550;
    lastUIHeight_ = 200;
}

AutoWahAudioProcessor::~AutoWahAudioProcessor()
{
    deallocateFilters();
}

//==============================================================================
const String AutoWahAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int AutoWahAudioProcessor::getNumParameters()
{
    return kNumParameters;
}

float AutoWahAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kBaseFrequencyParam:  return baseFrequency_;
        case kQParam:              return q_;
        case kLFOFrequencyParam:   return lfoFrequency_;
        case kLFOWidthParam:       return lfoWidth_;
        case kEnvelopeWidthParam:  return envelopeWidth_;
        case kEnvelopeAttackParam: return envelopeAttack_;
        case kEnvelopeDecayParam:  return envelopeDecay_;
        default:                   return 0.0f;
    }
}

void AutoWahAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    
    switch (index)
    {
        case kBaseFrequencyParam:
            baseFrequency_ = newValue;
            break;
        case kQParam:
            q_ = newValue;
            break;
        case kLFOFrequencyParam:
            lfoFrequency_ = newValue;
            break;
        case kLFOWidthParam:
            lfoWidth_ = newValue;
            break;
        case kEnvelopeWidthParam:
            envelopeWidth_ = newValue;
            break;
        case kEnvelopeAttackParam:
            envelopeAttack_ = newValue;
            // See comment below for justification
            if(envelopeAttack_ == 0.0)
                attackMultiplier_ = 0.0;
            else
                attackMultiplier_ = pow(1.0 / M_E, inverseSampleRate_ / envelopeAttack_);
            break;
        case kEnvelopeDecayParam:
            envelopeDecay_ = newValue;
            // envelopeDecay_ sets the time constant tau. The decay is
            // given as e^-(t/tau) so after tau seconds, it will have
            // decayed to 1/e of its original value. tau*sampleRate samples
            // will have passed by then, each of which multiplies the signal
            // by decayMultiplier_.
            if(envelopeDecay_ == 0.0)
                decayMultiplier_ = 0.0;
            else
                decayMultiplier_ = pow(1.0 / M_E, inverseSampleRate_ / envelopeDecay_);
            break;
        default:
            break;
    }
}

const String AutoWahAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case kBaseFrequencyParam:  return "base frequency";
        case kQParam:              return "Q";
        case kLFOFrequencyParam:   return "LFO frequency";
        case kLFOWidthParam:       return "LFO width";
        case kEnvelopeWidthParam:  return "envelope width";
        case kEnvelopeAttackParam: return "envelope attack";
        case kEnvelopeDecayParam:  return "envelope decay";
        default:                   break;
    }
    
    return String::empty;
}

const String AutoWahAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String AutoWahAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String AutoWahAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool AutoWahAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool AutoWahAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool AutoWahAudioProcessor::silenceInProducesSilenceOut() const
{
#if JucePlugin_SilenceInProducesSilenceOut
    return true;
#else
    return false;
#endif
}

double AutoWahAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

bool AutoWahAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AutoWahAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int AutoWahAudioProcessor::getNumPrograms()
{
    return 0;
}

int AutoWahAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AutoWahAudioProcessor::setCurrentProgram (int index)
{
}

const String AutoWahAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void AutoWahAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void AutoWahAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    allocateFilters();
    inverseSampleRate_ = 1.0 / sampleRate;
    if(envelopeDecay_ == 0.0)
        decayMultiplier_ = 0.0;
    else
        decayMultiplier_ = pow(1.0 / M_E, inverseSampleRate_ / envelopeDecay_);
    if(envelopeAttack_ == 0.0)
        attackMultiplier_ = 0.0;
    else
        attackMultiplier_ = pow(1.0 / M_E, inverseSampleRate_ / envelopeAttack_);
}

void AutoWahAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

    deallocateFilters();
}

void AutoWahAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Helpful information about this block of samples:
    const int numInputChannels = getNumInputChannels();     // How many input channels for our effect?
    const int numOutputChannels = getNumOutputChannels();   // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();          // How many samples in the buffer for this block?
    int channel;
    float ph;
    
    // Go through each channel and put it through the resonant lowpass filter, updating
    // the coefficients as we go along. Each channel is processed identically in this effect.
    
    for(channel = 0; channel < jmin(numInputChannels, numWahFilters_); ++channel)
    {
        // channelData is an array of length numSamples which contains the audio for one channel
        float* channelData = buffer.getWritePointer(channel);
        ph = lfoPhase_;
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float in = channelData[sample];
            float centreFrequency = baseFrequency_;
            
            // Calculate the envelope of the signal. Do this even if we're not currently
            // changing the frequeny based on it, since it involves maintaining a history
            // of the signal's behaviour.
            
            if(channel < numEnvelopes_) {   // Safety check
                if(fabs(in) > envelopes_[channel]) {
                    envelopes_[channel] += (1.0 - attackMultiplier_) * (fabs(in) - (double)envelopes_[channel]);
                }
                else
                    envelopes_[channel] *= decayMultiplier_;
            }
            
            // Calculate the centre frequency of the filter based on the LFO and the
            // signal envelope
            if(lfoWidth_ > 0.0) {
                centreFrequency += lfoWidth_ * (0.5f + 0.5f*sinf(2.0 * M_PI * ph));
            }
            if(envelopeWidth_ > 0.0 && channel < numEnvelopes_) {
                centreFrequency += envelopeWidth_ * envelopes_[channel];
            }
            
            // Update filter coefficients (see ResonantLowpassFilter.cpp for calculation)
            wahFilters_[channel]->makeResonantLowpass(inverseSampleRate_,
                                                      centreFrequency,
                                                      q_,
                                                      kWahwahFilterGain);
            
            // Process one sample and store it back in place. See juce_IIRFilter.cpp for the
            // application of the IIR filter.
            channelData[sample] = wahFilters_[channel]->processSingleSampleRaw(in);
            
            // Update the LFO phase, keeping it in the range 0-1
            ph += lfoFrequency_*inverseSampleRate_;
            if(ph >= 1.0)
                ph -= 1.0;
        }
    }
    
    lfoPhase_ = ph;
    
    // Go through the remaining channels. In case we have more outputs
    // than inputs, or there aren't enough filters, we'll clear any
    // remaining output channels (which could otherwise contain garbage)
    while(channel < numOutputChannels)
    {
        buffer.clear (channel++, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool AutoWahAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AutoWahAudioProcessor::createEditor()
{
    return new AutoWahAudioProcessorEditor (this);
}

//==============================================================================
void AutoWahAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // Create an outer XML element..
    XmlElement xml("C4DMPLUGINSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute("uiWidth", lastUIWidth_);
    xml.setAttribute("uiHeight", lastUIHeight_);
    xml.setAttribute("baseFrequency", baseFrequency_);
    xml.setAttribute("q", q_);
    xml.setAttribute("lfoFrequency", lfoFrequency_);
    xml.setAttribute("lfoWidth", lfoWidth_);
    xml.setAttribute("envelopeWidth", envelopeWidth_);
    xml.setAttribute("envelopeAttack", envelopeAttack_);
    xml.setAttribute("envelopeDecay", envelopeDecay_);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}

void AutoWahAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if(xmlState != 0)
    {
        // make sure that it's actually our type of XML object..
        if(xmlState->hasTagName("C4DMPLUGINSETTINGS"))
        {
            // ok, now pull out our parameters..
            lastUIWidth_  = xmlState->getIntAttribute("uiWidth", lastUIWidth_);
            lastUIHeight_ = xmlState->getIntAttribute("uiHeight", lastUIHeight_);
            
            q_ = (float)xmlState->getDoubleAttribute("q", q_);
            baseFrequency_ = (float)xmlState->getDoubleAttribute("baseFrequency", baseFrequency_);
            lfoFrequency_ = (float)xmlState->getDoubleAttribute("lfoFrequency", lfoFrequency_);
            lfoWidth_ = (float)xmlState->getDoubleAttribute("lfoWidth", lfoWidth_);
            envelopeWidth_ = (float)xmlState->getDoubleAttribute("envelopeWidth", envelopeWidth_);
            envelopeAttack_ = (float)xmlState->getDoubleAttribute("envelopeAttack", envelopeAttack_);
            envelopeDecay_ = (float)xmlState->getDoubleAttribute("envelopeDecay", envelopeDecay_);
            inverseSampleRate_ = 1.0 / getSampleRate();
            if(envelopeDecay_ == 0.0)
                decayMultiplier_ = 0.0;
            else
                decayMultiplier_ = pow(1.0 / M_E, inverseSampleRate_ / envelopeDecay_);
            if(envelopeAttack_ == 0.0)
                attackMultiplier_ = 0.0;
            else
                attackMultiplier_ = pow(1.0 / M_E, inverseSampleRate_ / envelopeAttack_);
        }
    }
}

void AutoWahAudioProcessor::allocateFilters()
{
    // Prevent leaks from reallocation
    if(wahFilters_ != 0 || envelopes_ != 0)
        deallocateFilters();
    
    // Create as many filters as we have input channels
    numWahFilters_ = getNumInputChannels();
    wahFilters_ = (ResonantLowpassFilter**)malloc(numWahFilters_ * sizeof(ResonantLowpassFilter*));
    if(wahFilters_ == 0)
        numWahFilters_ = 0;
    else {
        for(int i = 0; i < numWahFilters_; i++)
            wahFilters_[i] = new ResonantLowpassFilter;
    }
    
    numEnvelopes_ = getNumInputChannels();
    envelopes_ = (double *)malloc(numEnvelopes_ * sizeof(double));
    if(envelopes_ == 0)
        numEnvelopes_ = 0;
    else {
        for(int i = 0; i < numEnvelopes_; i++)
            envelopes_[i] = 0.0;
    }
}

void AutoWahAudioProcessor::deallocateFilters()
{
    for(int i = 0; i < numWahFilters_; i++)
        delete wahFilters_[i];
    if(numWahFilters_ != 0)
        free(wahFilters_);
    numWahFilters_ = 0;
    wahFilters_ = 0;
    if(envelopes_ != 0)
        free(envelopes_);
    envelopes_ = 0;
    numEnvelopes_ = 0;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutoWahAudioProcessor();
}
