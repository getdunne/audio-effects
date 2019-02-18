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

#include "TwoPoleAllpassFilter.h"

/* The actual audio processing is handled by the Juce IIRFilter parent
 * class. This subclass is used to define the coefficients for our
 * implementation of an allpass filter
 */

void TwoPoleAllpassFilter::makeAllpass (const double inverseSampleRate,
                                        const double centreFrequency,
                                        const double Q) noexcept
{
    jassert (Q > 0);

    const double discreteFrequency = 2.0 * M_PI * centreFrequency * inverseSampleRate;
    const double alpha = sin(discreteFrequency) / (2.0 * Q);
    const double cos_w0 = -2.0 * cos(discreteFrequency);
    
    // setCoefficients() takes arguments: b0, b1, b2, a0, a1, a2
    // It will normalise the filter according to the value of a0
    // to allow standard time-domain implementations
    
    coefficients = IIRCoefficients(1.0 - alpha, /* b0 */
                     cos_w0, /* b1 */
                     1.0 + alpha, /* b2 */
                     1.0 + alpha, /* a0 */
                     cos_w0, /* a1 */
                     1.0 - alpha /* a2 */);
    
    setCoefficients(coefficients);
}

/* Copy coefficients from another object of the same class */

void TwoPoleAllpassFilter::copyCoefficientsFrom (const TwoPoleAllpassFilter& other) noexcept
{
    setCoefficients(other.coefficients);
    active = other.active;
}