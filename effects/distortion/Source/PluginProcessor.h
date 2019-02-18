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

#ifndef __PLUGINPROCESSOR_H_88534BAA__
#define __PLUGINPROCESSOR_H_88534BAA__

#include "../JuceLibraryCode/JuceHeader.h"
#include <math.h>

class DistortionAudioProcessor  : public AudioProcessor
{
public:
    DistortionAudioProcessor();
    ~DistortionAudioProcessor();
    
    //==============================================================================
	// V S T   M E T H O D S

    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();
	void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
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
	// E N U M S

	enum Parameters
	{
        _PARAMdeviceReset,
        _PARAMgain,
        _PARAMtype,
        // add other parameters
		_PARAMtotalNumParams
	};
    
    
    enum Types
	{
        _hardClipping = 0,
        _softClipping,
        _softClippingExp,
        _fullWaveRectifier,
        _halfWaveRectifier,
        _numberOfTypes
	};

    //==============================================================================
	// O U R   M E T H O D S
    
    void Reset();

private:

	// Accessors and mutators
    inline float    GetGainIndB();
    inline void     SetGainIndB(float gainIndB);
    inline DistortionAudioProcessor::Types    GetType();
    inline void     SetType(Types type);

	int	_numChannels;
	int	_numSamples;
    int _sampleRate;
    float _gainIndB;
    
    Types _typeNumber;

	bool _isFirstFrame;

	// Buffers
	AudioSampleBuffer _currentTrackBuffer;

	int _lastUIWidth, _lastUIHeight;
    
	friend class DistortionAudioProcessorEditor;
    
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessor);
};

//============================================================================= 
// I N L I N E   F U N C T I O N S   C O D E   S E C T I O N 


//-----------------------------------------------------------------------------
//
inline float
DistortionAudioProcessor::GetGainIndB()
{
	return _gainIndB;
}


//-----------------------------------------------------------------------------
//
inline void
DistortionAudioProcessor::SetGainIndB(float gainIndB)
{
	_gainIndB = gainIndB;
}


//-----------------------------------------------------------------------------
//
inline DistortionAudioProcessor::Types
DistortionAudioProcessor::GetType()
{
	return _typeNumber;
}


//-----------------------------------------------------------------------------
//
inline void
DistortionAudioProcessor::SetType(Types type)
{
	_typeNumber = type;
}

#endif  // __PLUGINPROCESSOR_H_88534BAA__
