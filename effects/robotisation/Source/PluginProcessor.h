/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Robotisation: robot effect using phase vocoder
  See textbook Chapter 8: The Phase Vocoder
 
  Code by Andrew McPherson, Brecht De Man and Joshua Reiss
 
  This code requires the fftw library version 3 to compile:
  http://fftw.org
 
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

#ifndef __PLUGINPROCESSOR_H_4693CB6E__
#define __PLUGINPROCESSOR_H_4693CB6E__

#include "../JuceLibraryCode/JuceHeader.h"
#include <fftw3.h>

//==============================================================================
/**
*/
class RobotisationAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    RobotisationAudioProcessor();
    ~RobotisationAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;

    //==============================================================================
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

    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);
    
    //==============================================================================
    
    // these are used to persist the UI's size - the values are stored along with the
    // filter's other parameters, and the UI component will update them when it gets
    // resized.
    int lastUIWidth_, lastUIHeight_;
    
    enum Parameters
    {
        kFFTSizeParam = 0,
        kHopSizeParam,
        kWindowTypeParam,
        kNumParameters
    };
    
    enum Window
    {
        kWindowRectangular = 1,
        kWindowBartlett,
        kWindowHann,
        kWindowHamming
    };
    
    // This parameter indicates the FFT size for phase vocoder computation. It is selected
    // by the GUI and may temporarily differ from the actual size used in calculations.
    int fftSelectedSize_;
    int hopSelectedSize_;       // Hop size, as chosen by user
    int windowType_;            // Type of window used
    
private:
    // Methods to initialise and de-initialise the FFT machinery
    void initFFT(int length);
    void deinitFFT();
    
    // Methods to initialise and de-initialise the window
    void initWindow(int length, int windowType);
    void deinitWindow();
    
    // Methods to update the buffering for the given hop size and the output scaling
    void updateHopSize();
    void updateScaleFactor();
    
    // Whether the FFT has been initialised and is therefore ready to go
    bool fftInitialised_;
    
    // Variables for calculating the FFT and IFFT: complex data structures and the
    // "plan" used by the fftw library to calculate the transforms.
    fftw_complex *fftTimeDomain_, *fftFrequencyDomain_;
    fftw_plan fftForwardPlan_, fftBackwardPlan_;
    
    // Size of the FFT (generally a power of two) and the hop size (in samples, generally a fraction of FFT size)
    int fftActualTransformSize_;
    int hopActualSize_;
    
    // Amount by which to scale the inverse FFT to return to original amplitude: depends on the
    // transform size (because of fftw implementation) and the hop size (because of inherent overlap)
    double fftScaleFactor_;
    
    // Circular buffer gathers audio samples from the input until enough are available
    // for the FFT calculation
    AudioSampleBuffer inputBuffer_;
    int inputBufferLength_;
    int inputBufferWritePosition_;
    
    // Circular buffer that collects output samples from the FFT overlap-add process
    // before they are ready to be sent to the output stream
    AudioSampleBuffer outputBuffer_;
    int outputBufferLength_;
    int outputBufferReadPosition_, outputBufferWritePosition_;
    
    // How many samples since the last FFT?
    int samplesSinceLastFFT_;
    
    // Stored window function for pre-processing input frames
    double *windowBuffer_;
    int windowBufferLength_;
    
    // Whether or not prepareToPlay() has been called, i.e. that resources are in use
    bool preparedToPlay_;
    
    // Spin lock that prevents the FFT settings from changing in the middle of the audio
    // thread.
    SpinLock fftSpinLock_;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RobotisationAudioProcessor);
};

#endif  // __PLUGINPROCESSOR_H_4693CB6E__
