/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Compressor: dynamic range compression effect
  See textbook Chapter 6: Dynamics Processing
 
  Code by Joshua Reiss, Brecht de Man and Andrew McPherson
 
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
class CompressorAudioProcessor  : public AudioProcessor
{
public:
    CompressorAudioProcessor();
    ~CompressorAudioProcessor();
	
	int bufferSize;
	
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();
	void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
	
	void compressor(AudioSampleBuffer &buffer, int m);// compressor functions

	template <class T> const T& max ( const T& a, const T& b );

    AudioProcessorEditor* createEditor();

    bool hasEditor() const;

	AudioPlayHead::CurrentPositionInfo lastPosInfo;
 
	int round(float inn);
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

    bool silenceInProducesSilenceOut() const;
    double getTailLengthSeconds() const;
    bool acceptsMidi() const;
    bool producesMidi() const;

    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);

	float getThreshold();
	float getRatio();
	float getGain();
	float getAttackTime();
	float getReleaseTime();
	void setThreshold(float T);
	void setGain(float G);
	void setRatio(float R);
	void setAttackTime(float A);
	void setReleaseTime(float R);
	void resetAll();

	// parameters

	bool compressorONOFF;
	int M;
	bool autoTime;

private:
    AudioSampleBuffer inputBuffer;

//	int bufferSize;
    //these are used to persist UI's size- values are stored along with filter's other parameters, and UI component will update them when it gets resized.
	int lastUIWidth, lastUIHeight;
		
	HeapBlock <float> x_g, x_l,y_g, y_l,c;// input, output, control
		// parameters
	float ratio,threshold,makeUpGain,tauAttack,tauRelease,alphaAttack,alphaRelease,yL_prev;
	int nhost;
	int samplerate;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorAudioProcessor);
};

#endif  // __PLUGINPROCESSOR_H_88534BAA__
