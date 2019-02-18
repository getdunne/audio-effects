/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Parametric EQ: parametric equaliser adjusting frequency, Q and gain
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

#ifndef __PARAMETRICEQFILTER_H_6E48F605__
#define __PARAMETRICEQFILTER_H_6E48F605__

#define _USE_MATH_DEFINES
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
 * This class implements a biquad parametric EQ section according
 * to the equations in the Reiss and McPherson text.
 */

class ParametricEQFilter : public IIRFilter
{
public:
    //==============================================================================
	
	/* Makes a parametric EQ section entirely from discrete-time parameters.
     * Frequency here is specified from 0 to M_PI.
     */
	void makeParametric (const double discreteFrequency,
                         const double Q,
                         const double gainFactor) noexcept;
	
    /** Makes this filter duplicate the set-up of another one.
	 */
    void copyCoefficientsFrom (const ParametricEQFilter& other) noexcept;
	
private:
    //==============================================================================
	JUCE_LEAK_DETECTOR (ParametricEQFilter);
};


#endif // __PARAMETRICEQFILTER_H_6E48F605__