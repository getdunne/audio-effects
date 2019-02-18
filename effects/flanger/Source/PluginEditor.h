/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Flanger: flanging effect using time-varying delay
  See textbook Chapter 2: Delay Line Effects
 
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

#ifndef __PLUGINEDITOR_H_6E48F605__
#define __PLUGINEDITOR_H_6E48F605__

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================

class FlangerAudioProcessorEditor : public AudioProcessorEditor,
                                    public Slider::Listener,
                                    public ComboBox::Listener,
                                    public Button::Listener,
                                    public Timer
{
public:
    FlangerAudioProcessorEditor (FlangerAudioProcessor* ownerFilter);
    ~FlangerAudioProcessorEditor();

    //==============================================================================
    // This is just a standard Juce paint method...
    void timerCallback();
    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider*);
    void comboBoxChanged (ComboBox *);
    void buttonClicked (Button *);
    
private:
    Label delayLabel_, sweepWidthLabel_, depthLabel_;
    Label feedbackLabel_, frequencyLabel_, waveformLabel_;
    Label interpolationLabel_;
    Slider delaySlider_, sweepWidthSlider_, depthSlider_;
    Slider feedbackSlider_, frequencySlider_;
    ComboBox waveformComboBox_, interpolationComboBox_;
    ToggleButton stereoToggleButton_;
    
    ScopedPointer<ResizableCornerComponent> resizer_;
    ComponentBoundsConstrainer resizeLimits_;
    
    FlangerAudioProcessor* getProcessor() const
    {
        return static_cast <FlangerAudioProcessor*> (getAudioProcessor());
    }
};


#endif  // __PLUGINEDITOR_H_6E48F605__
