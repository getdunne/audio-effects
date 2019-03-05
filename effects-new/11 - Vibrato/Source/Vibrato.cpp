#include "Vibrato.h"


void Vibrato::populateInterpolationComboBox(ComboBox& cb)
{
    cb.clear(dontSendNotification);
    int itemNumber = 0;
    cb.addItem("Nearest Neighbor", ++itemNumber);
    cb.addItem("Linear", ++itemNumber);
    cb.addItem("Cubic", ++itemNumber);

}


float Vibrato::processSample(float sample, Type type, float delayReadPosition, float* delayData, int delayBufferLength, float interpolatedSample)
{
    switch (type)
    {
        case kInterpolationNearestNeighbour:
    {
        // Find the nearest input sample by rounding the fractional index to the
        // nearest integer. It's possible this will round it to the end of the buffer,
        // in which case we need to roll it back to the beginning.
      
        int closestSample = (int)floorf(delayReadPosition + 0.5);
        if(closestSample == delayBufferLength)
            closestSample = 0;
            interpolatedSample = delayData[closestSample];
        
    }
            
        case kInterpolationLinear:
    {
        // Find the fraction by which the read pointer sits between two
        // samples and use this to adjust weights of the samples
       
        float fraction = delayReadPosition - floorf(delayReadPosition);
        int previousSample = (int)floorf(delayReadPosition);
        int nextSample = (previousSample + 1) % delayBufferLength;
        interpolatedSample = fraction*delayData[nextSample]
        + (1.0f-fraction)*delayData[previousSample];
        
    }

        case kInterpolationCubic:
    {
            
        // Cubic interpolation will produce cleaner results at the expense
        // of more computation. This code uses the Catmull-Rom variant of
        // cubic interpolation. To reduce the load, calculate a few quantities
        // in advance that will be used several times in the equation:
        
        int sample1 = (int)floorf(delayReadPosition);
        int sample2 = (sample1 + 1) % delayBufferLength;
        int sample3 = (sample2 + 1) % delayBufferLength;
        int sample0 = (sample1 - 1 + delayBufferLength) % delayBufferLength;
        
        float fraction = delayReadPosition - floorf(delayReadPosition);
        float frsq = fraction*fraction;
        
        float a0 = -0.5f*delayData[sample0] + 1.5f*delayData[sample1]
        - 1.5f*delayData[sample2] + 0.5f*delayData[sample3];
        float a1 = delayData[sample0] - 2.5f*delayData[sample1]
        + 2.0f*delayData[sample2] - 0.5f*delayData[sample3];
        float a2 = -0.5f*delayData[sample0] + 0.5f*delayData[sample2];
        float a3 = delayData[sample1];
        
        interpolatedSample = a0*fraction*frsq + a1*frsq + a2*fraction + a3;
    }
        
        default:
            break;
            
            
            
            
            
            
            
            
    }
    
    return interpolatedSample;
}
