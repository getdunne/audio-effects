/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Auto-Wah: LFO or envelope-operated wah effect
  See textbook Chapter 4: Filter Effects
 
  Code by Andrew McPherson, Brecht de Man and Joshua Reiss
 
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

#include "ResonantLowpassFilter.h"

/* The actual audio processing is handled by the Juce IIRFilter parent
 * class. This subclass is used to define the coefficients for our
 * implementation of a parametric equaliser.
 */

void ResonantLowpassFilter::makeResonantLowpass(const double inverseSampleRate,
                                                const double centreFrequency,
                                                const double Q,
                                                const double gain) noexcept
{
    jassert (Q > 0);
    
    // Use inverse sample rate since multiplication is more efficient than
    // division, and the sample rate is unlikely to change each call.
    const double discreteFrequency = 2.0 * M_PI * centreFrequency * inverseSampleRate;
    const double alpha = sin(discreteFrequency) / (2.0 * Q);
    const double cos_w0 = cos(discreteFrequency);

    /* setCoefficients() takes arguments: b0, b1, b2, a0, a1, a2
     * It will normalise the filter according to the value of a0
     * to allow standard time-domain implementations
     */
    coefficients = IIRCoefficients(0.5*(1.0 - cos_w0)*gain, (1.0 - cos_w0)*gain, 0.5*(1.0 - cos_w0)*gain, 1.0 + alpha, -2.0 * cos_w0, 1.0 - alpha );
    
    setCoefficients(coefficients);
}

/* Copy coefficients from another object of the same class */

void ResonantLowpassFilter::copyCoefficientsFrom (const ResonantLowpassFilter& other) noexcept
{
    setCoefficients(other.coefficients);
    active = other.active;
}
