/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Phaser: phasing effect using time-varying allpass filters
  See textbook Chapter 4: Filter Effects
 
  Code by Andrew McPherson, Brecht De Man and Joshua Reiss
 
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

#ifndef __ONEPOLEALLPASSFILTER_H_6E48F605__
#define __ONEPOLEALLPASSFILTER_H_6E48F605__

#define _USE_MATH_DEFINES
#include "../JuceLibraryCode/JuceHeader.h"

/**
 * This class implements a 1st-order allpass filter used in the phaser effect.
 * Unlike TwoPoleAllpassFilter, it does not extend IIRFilter which is two-pole.
 * Loosely modelled after OnePoleFilter.cpp by dRowAudio.
 */

class OnePoleAllpassFilter
{
public:
    //==============================================================================
	// Create an unititialised filter.
    // This will not perform any filtering until the allpass coefficients have been set.
	OnePoleAllpassFilter() noexcept;
	
	// Destructor.
	~OnePoleAllpassFilter() noexcept;
	
	//Process a number of samples in one go.
    // This is the most effecient method of filtering.
    // Note that the samples passed to it actually get changed.
	void processSamples (float* const samples,
						 const int numSamples) noexcept;
	
	// Process a single sample.
    // Less efficient method but leaves the sample unchanged,
    // returning a filtered copy of it.
    float processSingleSampleRaw (const float sampleToProcess) noexcept;
	
	// Set the centre frequency of the allpass with respect to the sample rate
	void makeAllpass (const double inverseSampleRate,
                      const double centreFrequency) noexcept;
    
private:
    //==============================================================================
	CriticalSection lock;
	float x1, y1, b0, b1, a1;
	
    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OnePoleAllpassFilter);
};

#endif /* __ONEPOLEALLPASSFILTER_H_6E48F605__ */
