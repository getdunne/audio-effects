/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Robotisation: robot effect using phase vocoder;
  see also whisperisation code in processBlock() below.
 
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


#include "PluginProcessor.h"
#include "PluginEditor.h"

#ifdef WINDOWS
  #define M_PI       3.14159265358979323846
#endif

//==============================================================================
RobotisationAudioProcessor::RobotisationAudioProcessor() : inputBuffer_(2, 1), outputBuffer_(2, 1)
{
    // Set default values:
    fftSelectedSize_ = 512;
    hopSelectedSize_ = hopActualSize_ = 256;
    windowType_ = kWindowRectangular;

    fftInitialised_ = false;
    fftActualTransformSize_ = 0;
    inputBufferLength_ = 1;
    outputBufferLength_ = 1;
    inputBufferWritePosition_ = outputBufferWritePosition_ = outputBufferReadPosition_ = 0;
    samplesSinceLastFFT_ = 0;
    windowBuffer_ = 0;
    windowBufferLength_ = 0;
    preparedToPlay_ = false;
    fftScaleFactor_ = 0.0;
    
    lastUIWidth_ = 370;
    lastUIHeight_ = 120;
}

RobotisationAudioProcessor::~RobotisationAudioProcessor()
{
    // Release FFT resources if allocated. This should be handled by
    // releaseResources() but in the event it doesn't happen, this avoids
    // a leak. Harmless to call it twice.
    deinitFFT();
    deinitWindow();
}

//==============================================================================
const String RobotisationAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int RobotisationAudioProcessor::getNumParameters()
{
    return kNumParameters;
}

float RobotisationAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kFFTSizeParam:    return (float)fftSelectedSize_;
        case kHopSizeParam:    return (float)hopSelectedSize_;
        case kWindowTypeParam: return (float)windowType_;
        default:               return 0.0f;
    }
}

void RobotisationAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kFFTSizeParam:
            if((int)newValue != fftSelectedSize_)
            {
                fftSelectedSize_ = (int)newValue;
                if(preparedToPlay_)
                {
                    // Update settings if currently playing, else wait until prepareToPlay() called
                    initFFT(fftSelectedSize_);
                    initWindow(fftSelectedSize_, windowType_);
                }
            }
            break;
        case kHopSizeParam:
            hopSelectedSize_ = (int)newValue;
            if(preparedToPlay_)
                updateHopSize();
            break;
        case kWindowTypeParam:
            // Recalculate window if needed
            if((int)newValue != windowType_)
            {
                windowType_ = (int)newValue;
                if(preparedToPlay_)
                    initWindow(fftActualTransformSize_, (int)newValue);
            }
            break;
        default:
            break;
    }
}

const String RobotisationAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case kFFTSizeParam:    return "FFT size";
        case kHopSizeParam:    return "hop size";
        case kWindowTypeParam: return "window type";
        default:               break;
    }
    
    return String::empty;
}

const String RobotisationAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String RobotisationAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String RobotisationAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool RobotisationAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool RobotisationAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool RobotisationAudioProcessor::silenceInProducesSilenceOut() const
{
#if JucePlugin_SilenceInProducesSilenceOut
    return true;
#else
    return false;
#endif
}

double RobotisationAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

bool RobotisationAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool RobotisationAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int RobotisationAudioProcessor::getNumPrograms()
{
    return 0;
}

int RobotisationAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RobotisationAudioProcessor::setCurrentProgram (int index)
{
}

const String RobotisationAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void RobotisationAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void RobotisationAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    initFFT(fftSelectedSize_);
    initWindow(fftSelectedSize_, windowType_);
    preparedToPlay_ = true;
}

void RobotisationAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    
    deinitFFT();
    deinitWindow();
    preparedToPlay_ = false;
}

void RobotisationAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Helpful information about this block of samples:
    const int numInputChannels = getNumInputChannels();     // How many input channels for our effect?
    const int numOutputChannels = getNumOutputChannels();   // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();          // How many samples in the buffer for this block?
    
    int channel, inwritepos, sampsincefft;
    int outreadpos, outwritepos;
    
    // Grab the lock that prevents the FFT settings from changing
    fftSpinLock_.enter();
    
    // Check that we're initialised and ready to go. If not, set output to 0
    if(!fftInitialised_)
    {
        for (channel = 0; channel < numOutputChannels; ++channel)
        {
            buffer.clear (channel, 0, buffer.getNumSamples());
        }
        
        fftSpinLock_.exit();
        return;
    }

    // Go through each channel of audio that's passed in. Collect the samples in the input
    // buffer. When we've reached the next hop interval, calculate the FFT.
    for (channel = 0; channel < numInputChannels; ++channel)
    {
        // channelData is an array of length numSamples which contains the audio for one channel
        float* channelData = buffer.getSampleData(channel);
        
        // inputBufferData is the circular buffer for collecting input samples for the FFT
        float* inputBufferData = inputBuffer_.getSampleData(jmin (channel, inputBuffer_.getNumChannels() - 1));
        float* outputBufferData = outputBuffer_.getSampleData(jmin (channel, inputBuffer_.getNumChannels() - 1));
        
        // State variables need to be temporarily cached for each channel. We don't want the
        // operations on one channel to affect the identical behaviour of the next channel
        inwritepos = inputBufferWritePosition_;
        outwritepos = outputBufferWritePosition_;
        outreadpos = outputBufferReadPosition_;
        sampsincefft = samplesSinceLastFFT_;
        
        for (int i = 0; i < numSamples; ++i)
        {
            const float in = channelData[i];
            
            // Store the next buffered sample in the output. Do this first before anything
            // changes the output buffer-- we will have at least one FFT size worth of data
            // stored and ready to go. Set the result to 0 when finished in preparation for the
            // next overlap/add procedure.
            channelData[i] = outputBufferData[outreadpos];
            outputBufferData[outreadpos] = 0.0;
            if(++outreadpos >= outputBufferLength_)
                outreadpos = 0;

            // Store the current sample in the input buffer, incrementing the write pointer. Also
            // increment how many samples we've stored since the last transform. If it reaches the
            // hop size, perform an FFT and any frequency-domain processing.
            inputBufferData[inwritepos] = in;
            if (++inwritepos >= inputBufferLength_)
                inwritepos = 0;
            if (++sampsincefft >= hopActualSize_)
            {
                sampsincefft = 0;
                
                // Find the index of the starting sample in the buffer. When the buffer length
                // is equal to the transform size, this will be the current write position but
                // this code is more general for larger buffers.
                int inputBufferStartPosition = (inwritepos + inputBufferLength_
                                                - fftActualTransformSize_) % inputBufferLength_;
                
                // Window the buffer and copy it into the FFT input
                int inputBufferIndex = inputBufferStartPosition;
                for(int fftBufferIndex = 0; fftBufferIndex < fftActualTransformSize_; fftBufferIndex++)
                {
                    // Set real part to windowed signal; imaginary part to 0.
                    fftTimeDomain_[fftBufferIndex][1] = 0.0;
                    if(fftBufferIndex >= windowBufferLength_) // Safety check, in case window isn't ready
                        fftTimeDomain_[fftBufferIndex][0] = 0.0;
                    else
                        fftTimeDomain_[fftBufferIndex][0] = windowBuffer_[fftBufferIndex]
                                                                * inputBufferData[inputBufferIndex];
                    inputBufferIndex++;
                    if(inputBufferIndex >= inputBufferLength_)
                        inputBufferIndex = 0;
                }

                // Perform the FFT on the windowed data, going into the frequency domain.
                // Result will be in fftFrequencyDomain_
                fftw_execute(fftForwardPlan_);
                
                // ********** PHASE VOCODER PROCESSING GOES HERE **************
                // This is the place where frequency-domain calculations are made
                // on the transformed signal. Put the result back into fftFrequencyDomain_
                // before transforming back.
                // ************************************************************
                
                // Whisperiser
                /*for(int bin = 0; bin <= fftActualTransformSize_ / 2; bin++)
                {
                    float amplitude = sqrt(fftFrequencyDomain_[bin][0]*fftFrequencyDomain_[bin][0] +
                                           fftFrequencyDomain_[bin][1]*fftFrequencyDomain_[bin][1]);
                    
                    // This is what we would use to exactly reconstruct the signal:
                    // float phase = atan2(fftFrequencyDomain_[bin][1], fftFrequencyDomain_[bin][0]);
                    
                    // But instead, this is what we use to scramble the phase:
                    float phase = 2.0 * M_PI * (float)rand() / (float)RAND_MAX;

                    // Set the phase of each bin to 0. phase = 0 means the signal is entirely
                    // positive-real, but the overall amplitude is the same as before.
                    fftFrequencyDomain_[bin][0] = amplitude * cos(phase);
                    fftFrequencyDomain_[bin][1] = amplitude * sin(phase);
                    
                    // FFTs of real signals are conjugate-symmetric. We need to maintain that symmetry
                    // to produce a real output, even as we randomize the phase.
                    if(bin > 0 && bin < fftActualTransformSize_ / 2) {
                        fftFrequencyDomain_[fftActualTransformSize_ - bin][0] = amplitude * cos(phase);
                        fftFrequencyDomain_[fftActualTransformSize_ - bin][1] = - amplitude * sin(phase);
                    }
                }*/
                
                for(int bin = 0; bin < fftActualTransformSize_; bin++)
                {
                    float amplitude = sqrt(fftFrequencyDomain_[bin][0]*fftFrequencyDomain_[bin][0] +
                                           fftFrequencyDomain_[bin][1]*fftFrequencyDomain_[bin][1]);
                    
                    // Set the phase of each bin to 0. phase = 0 means the signal is entirely
                    // positive-real, but the overall amplitude is the same as before.
                    fftFrequencyDomain_[bin][0] = amplitude;
                    fftFrequencyDomain_[bin][1] = 0.0;
                }
                
                // Perform the inverse FFT to get back to the time domain. Result wll be
                // in fftTimeDomain_. If we've done it right (kept the frequency domain
                // symmetric), the time domain resuld should be strictly real allowing us
                // to ignore the imaginary part.
                fftw_execute(fftBackwardPlan_);
                
                // Add the result to the output buffer, starting at the current write position
                // (Output buffer will have been zeroed after reading the last time around)
                // Output needs to be scaled by the transform size to get back to original amplitude:
                // this is a property of how fftw is implemented. Scaling will also need to be adjusted
                // based on hop size to get the same output level (smaller hop size produces more overlap
                // and hence higher signal level)
                int outputBufferIndex = outwritepos;
                for(int fftBufferIndex = 0; fftBufferIndex < fftActualTransformSize_; fftBufferIndex++)
                {
                    // Reapply the window since phase changes will result in discontinities at the edges
                    // of the window
                    if(fftBufferIndex < windowBufferLength_)
                        outputBufferData[outputBufferIndex] += windowBuffer_[fftBufferIndex] *
                                                            fftTimeDomain_[fftBufferIndex][0] * fftScaleFactor_;
                    if(++outputBufferIndex >= outputBufferLength_)
                        outputBufferIndex = 0;
                }
                
                // Advance the write position within the buffer by the hop size
                outwritepos = (outwritepos + hopActualSize_) % outputBufferLength_;
            }
        }
    }
    
    // Having made a local copy of the state variables for each channel, now transfer the result
    // back to the main state variable so they will be preserved for the next call of processBlock()
    inputBufferWritePosition_ = inwritepos;
    outputBufferWritePosition_ = outwritepos;
    outputBufferReadPosition_ = outreadpos;
    samplesSinceLastFFT_ = sampsincefft;
    
    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = numInputChannels; i < numOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    fftSpinLock_.exit();
}

//==============================================================================
bool RobotisationAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* RobotisationAudioProcessor::createEditor()
{
    return new RobotisationAudioProcessorEditor (this);
}

//==============================================================================
void RobotisationAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // Create an outer XML element..
    XmlElement xml("C4DMPLUGINSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute("uiWidth", lastUIWidth_);
    xml.setAttribute("uiHeight", lastUIHeight_);
    xml.setAttribute("fftSize", fftSelectedSize_);
    xml.setAttribute("hopSize", hopSelectedSize_);
    xml.setAttribute("windowType", windowType_);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);
}

void RobotisationAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
            
            fftSelectedSize_  = (int)xmlState->getDoubleAttribute("fftSize", fftSelectedSize_);
            hopSelectedSize_  = (int)xmlState->getDoubleAttribute("hopSize", hopSelectedSize_);
            windowType_  = (int)xmlState->getDoubleAttribute("windowType", windowType_);
            
            if(preparedToPlay_)
            {
                // Update settings if currently playing, else wait until prepareToPlay() called
                initFFT(fftSelectedSize_);
                initWindow(fftSelectedSize_, windowType_);
            }
        }
    }
}

//==============================================================================
// Initialise the FFT data structures for a given length transform
void RobotisationAudioProcessor::initFFT(int length)
{
    if(fftInitialised_)
        deinitFFT();
    
    // Save the current length so we know how big our results are later
    fftActualTransformSize_ = length;
    
    // Here we allocate the complex-number buffers for the FFT. This uses
    // a convenient wrapper on the more general fftw_malloc()
    fftTimeDomain_ = fftw_alloc_complex(length);
    fftFrequencyDomain_ = fftw_alloc_complex(length);
    
    // FFTW_ESTIMATE doesn't necessarily produce the fastest executing code (FFTW_MEASURE
    // will get closer) but it carries a minimum startup cost. FFTW_MEASURE might stall for
    // several seconds which would be annoying in an audio plug-in context.
    fftForwardPlan_ = fftw_plan_dft_1d(fftActualTransformSize_, fftTimeDomain_,
                                       fftFrequencyDomain_, FFTW_FORWARD, FFTW_ESTIMATE);
    fftBackwardPlan_ = fftw_plan_dft_1d(fftActualTransformSize_, fftFrequencyDomain_,
                                       fftTimeDomain_, FFTW_BACKWARD, FFTW_ESTIMATE);
    
    // Allocate the buffer that the samples will be collected in
    inputBufferLength_ = fftActualTransformSize_;
    inputBuffer_.setSize(2, inputBufferLength_);
    inputBuffer_.clear();
    inputBufferWritePosition_ = 0;
    samplesSinceLastFFT_ = 0;
    
    // Allocate the output buffer to be twice the size of the FFT
    // This will be enough for all hop size cases
    outputBufferLength_ = 2*fftActualTransformSize_;
    outputBuffer_.setSize(2, outputBufferLength_);
    outputBuffer_.clear();
    outputBufferReadPosition_ = 0;
    
    updateHopSize();
    
    fftInitialised_ = true;
}

// Free the FFT data structures
void RobotisationAudioProcessor::deinitFFT()
{
    if(!fftInitialised_)
        return;
    
    // Prevent this variable from changing while an audio callback is running.
    // Once it has changed, the next audio callback will find that it's not
    // initialised and will return silence instead of attempting to work with the
    // (invalid) FFT structures. This produces an audible glitch but no crash,
    // and is the simplest way to handle parameter changes in this example code.
    fftSpinLock_.enter();
    fftInitialised_ = false;
    fftSpinLock_.exit();
    
    fftw_destroy_plan(fftForwardPlan_);
    fftw_destroy_plan(fftBackwardPlan_);
    fftw_free(fftTimeDomain_);
    fftw_free(fftFrequencyDomain_);
    
    // Leave the input buffer in memory until the plugin is released
}

//==============================================================================
// Create a new window of a given length and type
void RobotisationAudioProcessor::initWindow(int length, int windowType)
{
    if(windowBuffer_ != 0)
        deinitWindow();
    if(length == 0) // Sanity check
        return;
    
    // Allocate memory for the window
    windowBuffer_ = (double *)malloc(length * sizeof(double));
    
    // Write the length as a double here to simplify the code below (otherwise
    // typecasts would be wise)
    double windowLength = length;
    
    // Set values for the window, depending on its type
    for(int i = 0; i < length; i++)
    {
        // Window functions are typically defined to be symmetrical. This will cause a
        // problem in the overlap-add process: the windows instead need to be periodic
        // when arranged end-to-end. As a result we calculate the window of one sample
        // larger than usual, and drop the last sample. (This works as long as N is even.)
        // See Julius Smith, "Spectral Audio Signal Processing" for details.
        switch(windowType)
        {
            case kWindowBartlett:
                windowBuffer_[i] = (2.0/(windowLength + 2.0))*
                    (0.5*(windowLength + 2.0) - abs((double)i - 0.5*windowLength));
                break;
            case kWindowHann:
                windowBuffer_[i] = 0.5*(1.0 - cos(2.0*M_PI*(double)i/windowLength));
                break;
            case kWindowHamming:
                windowBuffer_[i] = 0.54 - 0.46*cos(2.0*M_PI*(double)i/windowLength);
                break;
            case kWindowRectangular:
            default:
                windowBuffer_[i] = 1.0;
                break;
        }
    }
    
    windowBufferLength_ = length;
    updateScaleFactor();
}

// Free the window buffer
void RobotisationAudioProcessor::deinitWindow()
{
    if(windowBuffer_ == 0)
        return;
    
    // Delay clearing the window until the audio thread is not running
    // to avoid a crash if the code tries to access an invalid window
    fftSpinLock_.enter();
    windowBufferLength_ = 0;
    fftSpinLock_.exit();
    
    free(windowBuffer_);
    windowBuffer_ = 0;
}

// Update the actual hop size depending on the window size and hop size settings
// Hop size is expressed as a fraction of a window in the parameters.
void RobotisationAudioProcessor::updateHopSize()
{
    hopActualSize_ = hopSelectedSize_;
    
    // Update the factor by which samples are scaled to preserve unity gain
    updateScaleFactor();
    
    // Read pointer lags the write pointer to allow for FFT buffers to accumulate and
    // be processed. Total latency is sum of the FFT size and the hop size.
    outputBufferWritePosition_ = hopActualSize_ + fftActualTransformSize_;
}

// Update the factor by which each output sample is scaled. This needs to update
// every time FFT size, hop size, and window type are changed.
void RobotisationAudioProcessor::updateScaleFactor()
{
    // The gain needs to be normalised by the sum of the window, which implicitly
    // accounts for the length of the transform and the window type. From there
    // we also update based on hop size: smaller hop means more overlap means the
    // overall gain should be reduced.
    double windowSum = 0.0;
    
    for(int i = 0; i < windowBufferLength_; i++)
    {
        windowSum += windowBuffer_[i];
    }
    
    if(windowSum == 0.0)
        fftScaleFactor_ = 0.0; // Catch invalid cases and mute output
    else
    {
        fftScaleFactor_ = ((float)hopSelectedSize_/(float)fftActualTransformSize_)/(double)windowSum;
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RobotisationAudioProcessor();
}
