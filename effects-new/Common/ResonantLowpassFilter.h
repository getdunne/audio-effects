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

#pragma once
#include "JuceHeader.h"

/**
 * This class implements a resonant lowpass filter used in the wah-wah
 * effect and its variants
 */

class ResonantLowpassFilter : public IIRFilter
{
public:
    ResonantLowpassFilter() : IIRFilter() {}

	// Sets the filter coefficients to produce a resonant lowpass with a given
    // centre frequency and Q. Also include a gain term to offset resonant peak.
	void makeResonantLowpass(const double inverseSampleRate,
                             const double centreFrequency,
                             const double Q,
                             const double gain) noexcept;
	
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResonantLowpassFilter);
};
