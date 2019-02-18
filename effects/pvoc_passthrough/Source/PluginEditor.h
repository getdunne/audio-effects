/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  PVOC Passthrough: phase vocoder structure which passes input
  to output without performing any processing
 
  See textbook Chapter 8: The Phase Vocoder
 
  Code by Andrew McPherson, Brecht De Man and Joshua Reiss
 
  This code requires the fftw library version 3 to compile:
  http://fftw.org
 
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


#ifndef __PLUGINEDITOR_H_6E48F605__
#define __PLUGINEDITOR_H_6E48F605__

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================

class PVOCPassthroughAudioProcessorEditor  : public AudioProcessorEditor,
                                   public Slider::Listener,
                                   public ComboBox::Listener,
                                   public Timer
{
public:
    PVOCPassthroughAudioProcessorEditor (PVOCPassthroughAudioProcessor* ownerFilter);
    ~PVOCPassthroughAudioProcessorEditor();

    //==============================================================================
    // This is just a standard Juce paint method...
    void timerCallback();
    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider*);
    void comboBoxChanged (ComboBox *);
    
private:
    Label fftSizeLabel_, hopSizeLabel_, windowTypeLabel_;
    ComboBox fftSizeComboBox_, hopSizeComboBox_, windowTypeComboBox_;
    
    ScopedPointer<ResizableCornerComponent> resizer_;
    ComponentBoundsConstrainer resizeLimits_;
    
    PVOCPassthroughAudioProcessor* getProcessor() const
    {
        return static_cast <PVOCPassthroughAudioProcessor*> (getAudioProcessor());
    }
};


#endif  // __PLUGINEDITOR_H_6E48F605__
