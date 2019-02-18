/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Wah-Wah: filter effect using variable-frequency filter
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


#ifndef __ResonantLowpassFilter_H_6E48F605__
#define __ResonantLowpassFilter_H_6E48F605__

#define _USE_MATH_DEFINES
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
 * This class implements a resonant lowpass filter used in the wah-wah
 * effect and its variants
 */

class ResonantLowpassFilter : public IIRFilter
{
public:
    //==============================================================================
	
	/* Sets the filter coefficients to produce a resonant lowpass with a given
     * centre frequency and Q. Also include a gain term to offset resonant peak.
     */
	void makeResonantLowpass(const double inverseSampleRate,
                             const double centreFrequency,
                             const double Q,
                             const double gain) noexcept;
	
    /** Makes this filter duplicate the set-up of another one.
	 */
    void copyCoefficientsFrom (const ResonantLowpassFilter& other) noexcept;
	
private:
    //==============================================================================
	JUCE_LEAK_DETECTOR (ResonantLowpassFilter);
};


#endif // __ResonantLowpassFilter_H_6E48F605__