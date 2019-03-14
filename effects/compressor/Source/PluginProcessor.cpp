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

#include "PluginProcessor.h"
#include "PluginEditor.h"
CompressorAudioProcessor::CompressorAudioProcessor()
	// Initializer List
	:
	inputBuffer(1,1),
	nhost(0)
{
	lastUIWidth = 850; 
    lastUIHeight = 650;
    lastPosInfo.resetToDefault();
}
CompressorAudioProcessor::~CompressorAudioProcessor()
{
}
//==============================================================================
void CompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback initialisation that you need.
	M = round(getNumInputChannels()/2);
	samplerate = (float)getSampleRate();
	bufferSize = getBlockSize();
	// Allocate a lot of dynamic memory here
	x_g					.allocate(bufferSize, true);
	x_l					.allocate(bufferSize, true);			
	y_g					.allocate(bufferSize, true);				
	y_l					.allocate(bufferSize, true);	
	c					.allocate(bufferSize, true);
	yL_prev=0;
	autoTime = false;
	compressorONOFF = false;
	resetAll();
}
void CompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this to free up any spare memory, etc.
}
void CompressorAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	if (compressorONOFF)
	{
		inputBuffer.setSize(M,bufferSize);
		inputBuffer.clear();
		for (int m = 0 ; m < M ; ++m)
		{
			if ( (threshold< 0) )
			{
				inputBuffer.clear(m,0,bufferSize);
				// Mix down left-right to analyse the input		
				inputBuffer.addFrom(m,0,buffer,m*2,0,bufferSize,0.5);
				inputBuffer.addFrom(m,0,buffer,m*2+1,0,bufferSize,0.5);
				// compression : calculates the control voltage
				compressor(inputBuffer,m);
				// apply control voltage to the audio signal
				for (int i = 0 ; i < bufferSize ; ++i)
				{
					buffer.getWritePointer(2*m+0)[i] *= c[i];
					buffer.getWritePointer(2*m+1)[i] *= c[i];
				}
				inputBuffer.clear(m,0,bufferSize);
				// Mix down left-right to analyse the output
				inputBuffer.addFrom(m,0,buffer,m*2,0,bufferSize,0.5);
				inputBuffer.addFrom(m,0,buffer,m*2+1,0,bufferSize,0.5);
			}
		}
	}
}
// compressor functions
void CompressorAudioProcessor::compressor(AudioSampleBuffer &buffer, int m)
{
	alphaAttack = exp(-1/(0.001 * samplerate * tauAttack));
	alphaRelease= exp(-1/(0.001 * samplerate * tauRelease));
	for (int i = 0 ; i < bufferSize ; ++i)
	{
		//Level detection- estimate level using peak detector
		if (fabs(buffer.getWritePointer(m)[i]) < 0.000001) x_g[i] =-120;
		else x_g[i] =20*log10(fabs(buffer.getWritePointer(m)[i]));
		//Gain computer- static apply input/output curve
		if (x_g[i] >= threshold) y_g[i] = threshold+ (x_g[i] - threshold) / ratio;
		else y_g[i] = x_g[i];
		x_l[i] = x_g[i] - y_g[i];
		//Ballistics- smoothing of the gain 
		if (x_l[i]>yL_prev)  y_l[i]=alphaAttack * yL_prev+(1 - alphaAttack ) * x_l[i] ; 
		else				 y_l[i]=alphaRelease* yL_prev+(1 - alphaRelease) * x_l[i] ;
		//find control
		c[i] = pow(10,(makeUpGain - y_l[i])/20);
		yL_prev=y_l[i];
	}
}
template <class T> const T& CompressorAudioProcessor::max( const T& a, const T& b )
{
  return (a < b) ? b : a;
}
void CompressorAudioProcessor::resetAll()
{
		tauAttack=0;tauRelease = 0;
		alphaAttack=0;alphaRelease = 0;
		threshold = 0;
		ratio= 1;
		makeUpGain= 0;
		yL_prev=0;
	for (int i = 0 ; i < bufferSize ; ++i)
	{
		x_g[i] = 0;	y_g[i] = 0;
		x_l[i] = 0;	y_l[i] = 0;	
		c[i] = 0;
	}
}
//////////////////////////////////////////////
float CompressorAudioProcessor::getThreshold()
{
	return threshold;
}
float CompressorAudioProcessor::getRatio()
{
	return ratio;
}
float CompressorAudioProcessor::getGain()
{
	return makeUpGain;//problem?
}
float CompressorAudioProcessor::getAttackTime()
{
	return tauAttack;
}
float CompressorAudioProcessor::getReleaseTime()
{
	return tauRelease;
}
////////////////////////////////////////////////////////
void CompressorAudioProcessor::setThreshold(float T)
{
	threshold= T;
}
void CompressorAudioProcessor::setGain(float G)
{
	makeUpGain= G;
}
void CompressorAudioProcessor::setRatio(float R)
{
	ratio= R;
}
void CompressorAudioProcessor::setAttackTime(float A)
{
	tauAttack = A;
}
void CompressorAudioProcessor::setReleaseTime(float R)
{
	tauRelease = R;
}
bool CompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}
AudioProcessorEditor* CompressorAudioProcessor::createEditor()
{
    return new CompressorAudioProcessorEditor (this);
}
//==============================================================================
void CompressorAudioProcessor::getStateInformation (MemoryBlock& destData)
{
//Use this to store your parameters in memory block, either as raw data, or use XML or ValueTree classes as intermediaries to make it easy to save and load complex data.
}
void CompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
// Use this to restore your parameters from this memory block, whose contents will have been created by the getStateInformation() call.
}
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CompressorAudioProcessor();
}
int CompressorAudioProcessor::round(float inn)
{
	if (inn > 0) return (int) (inn + 0.5);
	else return (int) (inn - 0.5);
}
const String CompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}
int CompressorAudioProcessor::getNumParameters()
{
    return 0;
}
float CompressorAudioProcessor::getParameter (int index)
{
    return 0.0f;
}
void CompressorAudioProcessor::setParameter (int index, float newValue)
{
}
const String CompressorAudioProcessor::getParameterName (int index)
{
    return String::empty;
}
const String CompressorAudioProcessor::getParameterText (int index)
{
    return String::empty;
}
const String CompressorAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}
const String CompressorAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}
bool CompressorAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}
bool CompressorAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}
bool CompressorAudioProcessor::silenceInProducesSilenceOut() const
{
#if JucePlugin_SilenceInProducesSilenceOut
    return true;
#else
    return false;
#endif
}

double CompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}
bool CompressorAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}
bool CompressorAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}
int CompressorAudioProcessor::getNumPrograms()
{
    return 0;
}
int CompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}
void CompressorAudioProcessor::setCurrentProgram (int index)
{
}
const String CompressorAudioProcessor::getProgramName (int index)
{
    return String::empty;
}
void CompressorAudioProcessor::changeProgramName (int index, const String& newName)
{
}