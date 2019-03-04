#include "ResonantLowpassFilter.h"
#define PI_D 3.141592654

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
    const double discreteFrequency = 2.0 * PI_D * centreFrequency * inverseSampleRate;
    const double alpha = sin(discreteFrequency) / (2.0 * Q);
    const double cos_w0 = cos(discreteFrequency);

    // setCoefficients() takes arguments: b0, b1, b2, a0, a1, a2
    // It will normalise the filter according to the value of a0
    // to allow standard time-domain implementations
    coefficients = IIRCoefficients(0.5*(1.0 - cos_w0)*gain, // b0
                     (1.0 - cos_w0)*gain,                   // b1
                     0.5*(1.0 - cos_w0)*gain,               // b2
                     1.0 + alpha,                           // a0
                     -2.0 * cos_w0,                         // a1
                     1.0 - alpha );                         // a2
    
    setCoefficients(coefficients);
}
