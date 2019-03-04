#include "ParametricEQFilter.h"
#define PI_D 3.141592654

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
    
    // Limit the bandwidth so we don't get a nonsense result from tan(B/2)
    const double bandwidth = jmin(discreteFrequency / Q, PI_D * 0.99);
    const double two_cos_wc = -2.0*cos(discreteFrequency);
    const double tan_half_bw = tan(bandwidth / 2.0);
    const double g_tan_half_bw = gainFactor * tan_half_bw;
    const double sqrt_g = sqrt(gainFactor);
    
    // setCoefficients() takes arguments: b0, b1, b2, a0, a1, a2
    //It will normalise the filter according to the value of a0
    //to allow standard time-domain implementations
    coefficients = IIRCoefficients(sqrt_g + g_tan_half_bw,  // b0
                                   sqrt_g * two_cos_wc,     // b1
                                   sqrt_g - g_tan_half_bw,  // b2
                                   sqrt_g + tan_half_bw,    // a0
                                   sqrt_g * two_cos_wc,     // a1
                                   sqrt_g - tan_half_bw);   // a2
    
    setCoefficients(coefficients);
}
