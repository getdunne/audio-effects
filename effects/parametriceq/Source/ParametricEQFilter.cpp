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

#include "ParametricEQFilter.h"

/* The actual audio processing is handled by the Juce IIRFilter parent
 * class. This subclass is used to define the coefficients for our
 * implementation of a parametric equaliser.
 */

void ParametricEQFilter::makeParametric(const double discreteFrequency,
                                        const double Q,
                                        const double gainFactor) noexcept
{
    jassert (discreteFrequency > 0);
    jassert (Q > 0);
    
    /* Limit the bandwidth so we don't get a nonsense result from tan(B/2) */
    const double bandwidth = jmin(discreteFrequency / Q, M_PI * 0.99);
    const double two_cos_wc = -2.0*cos(discreteFrequency);
    const double tan_half_bw = tan(bandwidth / 2.0);
    const double g_tan_half_bw = gainFactor * tan_half_bw;
    const double sqrt_g = sqrt(gainFactor);
    
    /* setCoefficients() takes arguments: b0, b1, b2, a0, a1, a2
     * It will normalise the filter according to the value of a0
     * to allow standard time-domain implementations
     */
    
    coefficients = IIRCoefficients(sqrt_g + g_tan_half_bw, /* b0 */
                     sqrt_g * two_cos_wc, /* b1 */
                     sqrt_g - g_tan_half_bw, /* b2 */
                     sqrt_g + tan_half_bw, /* a0 */
                     sqrt_g * two_cos_wc, /* a1 */
                     sqrt_g - tan_half_bw /* a2 */);
    
    setCoefficients(coefficients);
}

/* Copy coefficients from another object of the same class */

void ParametricEQFilter::copyCoefficientsFrom (const ParametricEQFilter& other) noexcept
{
    setCoefficients(other.coefficients);
    active = other.active;
}