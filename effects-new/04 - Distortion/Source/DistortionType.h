/*
  ==============================================================================

    DistortionType.h
    Created: 2 Mar 2019 7:54:07pm
    Author:  Michael Cassidy

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class DistortionType   
{
public:
    DistortionType()
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.

    }

    ~DistortionType();

    
float DistortionType::Type (int _typeNumber,float *_originalData, float  *_buffer){

    switch (_typeNumber) {
        case _hardClipping:
        {
            float threshold = 0.5f;
            for (int sample = 0; sample < *_buffer->getNumSamples(); ++sample)
            {
                if(_originalData[sample]  > threshold) // positive hard clipping
                {
                    _originalData[sample] = threshold;
                }
                else
                {
                    if(_originalData[sample]  < - threshold) // negative hard clipping
                    {
                        _originalData[sample] = - threshold;
                    }
                }
            }
            break;
        }
            
        case _softClipping:
        {
            float threshold1 = 1.0f/3.0f;
            float threshold2 = 2.0f/3.0f;
            for (int sample = 0; sample < *_buffer->getNumSamples(); ++sample)
            {
                if(_originalData[sample]  > threshold1)
                {
                    if(_originalData[sample] > threshold2) // positive clipping
                    {
                        _originalData[sample] = 1.0f;
                    }
                    else // soft knee (positive)
                    {
                        _originalData[sample] = (3.0f - (2.0f - 3.0f*_originalData[sample])*(2.0f - 3.0f*_originalData[sample]))/3.0f;
                    }
                }
                else
                {
                    if(_originalData[sample]  < -threshold1)
                    {
                        if(_originalData[sample] < -threshold2) // negative clipping
                        {
                            _originalData[sample] = -1.0f;
                        }
                        else // soft knee (negative)
                        {
                            _originalData[sample] = - (3.0f - (2.0f + 3.0f*_originalData[sample])*(2.0f + 3.0f*_originalData[sample]))/3.0f;
                        }
                    }
                    else // linear region (-1/3..1/3)
                    {
                        _originalData[sample] *= 2.0f;
                    }
                }
                
                _originalData[sample] /= 2.0f; // divide all by 2 to compensate for extra 6 dB gain boost
            }
            break;
        }
            
        case _softClippingExp:
        {
            for (int sample = 0; sample < *_buffer->getNumSamples(); ++sample)
            {
                if (_originalData[sample] > 0.0f) // positive
                {
                   _originalData[sample] = 1.0f - expf(-_originalData[sample]);
                }
                else // negative
                {
                    _originalData[sample] = - 1.0f + expf(_originalData[sample]);
                }
            }
            break;
        }
            
        case _fullWaveRectifier:
        {
            for (int sample = 0; sample < *_buffer->getNumSamples(); ++sample)
            {
                _originalData[sample] = fabs(_originalData[sample]);
            }
            break;
        }
            
        case _halfWaveRectifier:
        {
            for (int sample = 0; sample < *_buffer->getNumSamples(); ++sample)
            {
                _originalData[sample] = 0.5f*(fabs(_originalData[sample])+_originalData[sample]);
            }
            break;
        }
            
        default:
            break;
    }
    
}
    enum Types
    {
        _hardClipping = 0,
        _softClipping,
        _softClippingExp,
        _fullWaveRectifier,
        _halfWaveRectifier,
        _numberOfTypes
    };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionType)
};
