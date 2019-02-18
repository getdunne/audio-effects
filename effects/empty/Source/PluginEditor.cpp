/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Empty: template for an effect; passes input to output unmodified
  See textbook Chapter 13: Building Audio Effect Plug-Ins
 
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

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
EmptyAudioProcessorEditor::EmptyAudioProcessorEditor (EmptyAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      volumeLabel_("", "Volume:"),
      volumeSlider_("volume")
{
    // This is where our plugin's editor size is set.
    // setSize(170, 80);
    
    // Set up the sliders
    addAndMakeVisible(&volumeSlider_);
    volumeSlider_.setSliderStyle(Slider::Rotary);
    volumeSlider_.addListener(this);
    volumeSlider_.setRange(0.0, 2.0, 0.01);
    
    volumeLabel_.attachToComponent(&volumeSlider_, false);
    volumeLabel_.setFont(Font (11.0f));
    
    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible(resizer_ = new ResizableCornerComponent (this, &resizeLimits_));
    resizeLimits_.setSizeLimits(170, 100, 400, 160);
    
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(ownerFilter->lastUIWidth_,
            ownerFilter->lastUIHeight_);
    
    startTimer(50);
}

EmptyAudioProcessorEditor::~EmptyAudioProcessorEditor()
{
}

//==============================================================================
void EmptyAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
}

void EmptyAudioProcessorEditor::resized()
{
    volumeSlider_.setBounds(20, 20, 150, 40);
    
    resizer_->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
    
    getProcessor()->lastUIWidth_ = getWidth();
    getProcessor()->lastUIHeight_ = getHeight();
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void EmptyAudioProcessorEditor::timerCallback()
{
    EmptyAudioProcessor* ourProcessor = getProcessor();
    
    volumeSlider_.setValue(ourProcessor->volume_, dontSendNotification);
}

// This is our Slider::Listener callback, when the user drags a slider.
void EmptyAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (slider == &volumeSlider_)
    {
        // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
        // by the host, rather than just modifying them directly, otherwise the host won't know
        // that they've changed.
        getProcessor()->setParameterNotifyingHost (EmptyAudioProcessor::kVolumeParam,
                                                   (float)volumeSlider_.getValue());
    }
}