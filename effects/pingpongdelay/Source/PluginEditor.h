/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Ping-Pong Delay: stereo delay alternating between channels
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

class PingPongDelayAudioProcessorEditor  : public AudioProcessorEditor,
                                   public Slider::Listener,
                                   public Button::Listener,
                                   public Timer
{
public:
    PingPongDelayAudioProcessorEditor (PingPongDelayAudioProcessor* ownerFilter);
    ~PingPongDelayAudioProcessorEditor();

    //==============================================================================
    // This is just a standard Juce paint method...
    void timerCallback();
    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider*);
    void buttonClicked (Button *);
    
private:
    Label delayLengthLeftLabel_, delayLengthRightLabel_;
    Label feedbackLabel_, wetMixLabel_;
    Slider delayLengthLeftSlider_, delayLengthRightSlider_;
    Slider feedbackSlider_, wetMixSlider_;
    ToggleButton linkDelaysButton_;
    ToggleButton reverseChannelsButton_;
    
    bool linkDelays_, reverseChannels_;
    
    ScopedPointer<ResizableCornerComponent> resizer_;
    ComponentBoundsConstrainer resizeLimits_;
    
    PingPongDelayAudioProcessor* getProcessor() const
    {
        return static_cast <PingPongDelayAudioProcessor*> (getAudioProcessor());
    }
};


#endif  // __PLUGINEDITOR_H_6E48F605__
