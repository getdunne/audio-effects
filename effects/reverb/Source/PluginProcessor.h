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


#ifndef __PLUGINPROCESSOR_H_88534BAA__
#define __PLUGINPROCESSOR_H_88534BAA__

#include "../JuceLibraryCode/JuceHeader.h"
#include "MVerb.h"
#include <math.h>

class ReverbAudioProcessor  : public AudioProcessor
{
public:
    ReverbAudioProcessor();
    ~ReverbAudioProcessor();
    
    //==============================================================================
	// V S T   M E T H O D S

    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();
	void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
    void updateParameters(int index);
    AudioProcessorEditor* createEditor();
	bool silenceInProducesSilenceOut() const;
	virtual double getTailLengthSeconds() const {return 0;};
    bool hasEditor() const;
    const String getName() const;
    int getNumParameters();
    float getParameter (int index);
    void setParameter (int index, float newValue);
    const String getParameterName (int index);
    const String getParameterText (int index);
    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;
    bool acceptsMidi() const;
    bool producesMidi() const;
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);
    

    //==============================================================================
	// O U R   M E T H O D S
    
    void Reset();

private:

	// Accessors and mutators
    inline float    GetDensity();
    inline void     SetDensity(float density);
    inline float    GetDecay();
    inline void     SetDecay(float decay);
    inline float    GetSize();
    inline void     SetSize(float size);
    inline float    GetDamp();
    inline void     SetDamp(float damp);
    inline float    GetBandwidth();
    inline void     SetBandwidth(float bandwith);
    inline float    GetPredelay();
    inline void     SetPredelay(float predelay);
    inline float    GetGain();
    inline void     SetGain(float gain);
    inline float    GetMix();
    inline void     SetMix(float mix);
    inline float    GetLateEarly();
    inline void     SetLateEarly(float lateEarly);

	int	_numChannels;
	int	_numSamples;
    int _sampleRate;
    
    // parameters
    float _density;
    float _decay;
    float _size;
    float _damp;
    float _bandwidth;
    float _predelay;
    float _gain;
    float _mix;
    float _lateEarly;
    
    MVerb<float> _mverb;
    

	bool _isFirstFrame;

	// Buffers
	AudioSampleBuffer tempInput, tempOutput;

	int _lastUIWidth, _lastUIHeight;
    
	friend class ReverbAudioProcessorEditor;
    
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbAudioProcessor);
};

//============================================================================= 
// I N L I N E   F U N C T I O N S   C O D E   S E C T I O N 

//-----------------------------------------------------------------------------
//
inline float
ReverbAudioProcessor::GetDensity()
{
	return _density;
}


//-----------------------------------------------------------------------------
//
inline void
ReverbAudioProcessor::SetDensity(float density)
{
	_density = density;
}


//-----------------------------------------------------------------------------
//
inline float
ReverbAudioProcessor::GetDecay()
{
	return _decay;
}


//-----------------------------------------------------------------------------
//
inline void
ReverbAudioProcessor::SetDecay(float decay)
{
	_decay = decay;
}

//-----------------------------------------------------------------------------
//
inline float
ReverbAudioProcessor::GetSize()
{
	return _size;
}


//-----------------------------------------------------------------------------
//
inline void
ReverbAudioProcessor::SetSize(float size)
{
	_size = size;
}

//-----------------------------------------------------------------------------
//
inline float
ReverbAudioProcessor::GetDamp()
{
	return _damp;
}


//-----------------------------------------------------------------------------
//
inline void
ReverbAudioProcessor::SetDamp(float damp)
{
	_damp = damp;
}

//-----------------------------------------------------------------------------
//
inline float
ReverbAudioProcessor::GetBandwidth()
{
	return _bandwidth;
}


//-----------------------------------------------------------------------------
//
inline void
ReverbAudioProcessor::SetBandwidth(float bandwidth)
{
	_bandwidth = bandwidth;
}

//-----------------------------------------------------------------------------
//
inline float
ReverbAudioProcessor::GetPredelay()
{
	return _predelay;
}


//-----------------------------------------------------------------------------
//
inline void
ReverbAudioProcessor::SetPredelay(float predelay)
{
	_predelay = predelay;
}

//-----------------------------------------------------------------------------
//
inline float
ReverbAudioProcessor::GetGain()
{
	return _gain;
}


//-----------------------------------------------------------------------------
//
inline void
ReverbAudioProcessor::SetGain(float gain)
{
	_gain = gain;
}

//-----------------------------------------------------------------------------
//
inline float
ReverbAudioProcessor::GetMix()
{
	return _mix;
}


//-----------------------------------------------------------------------------
//
inline void
ReverbAudioProcessor::SetMix(float mix)
{
	_mix = mix;
}

//-----------------------------------------------------------------------------
//
inline float
ReverbAudioProcessor::GetLateEarly()
{
	return _lateEarly;
}


//-----------------------------------------------------------------------------
//
inline void
ReverbAudioProcessor::SetLateEarly(float lateEarly)
{
	_lateEarly = lateEarly;
}
#endif  // __PLUGINPROCESSOR_H_88534BAA__
