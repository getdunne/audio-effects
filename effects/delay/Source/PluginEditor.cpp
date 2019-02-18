/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Delay: basic delay effect with feedback
  See textbook Chapter 2: Delay Line Effects
 
  Code by Andrew McPherson, Brecht de Man and Joshua Reiss
 
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
DelayAudioProcessorEditor::DelayAudioProcessorEditor (DelayAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      delayLengthLabel_("", "Delay (sec):"),
      feedbackLabel_("", "Feedback:"),
      dryMixLabel_("", "Dry Mix Level:"),
      wetMixLabel_("", "Delayed Mix Level:")
{

    // Set up the sliders
    addAndMakeVisible (&delayLengthSlider_);
    delayLengthSlider_.setSliderStyle (Slider::Rotary);
    delayLengthSlider_.addListener (this);
    delayLengthSlider_.setRange (0.01, 2.0, 0.01);
    
    addAndMakeVisible (&feedbackSlider_);
    feedbackSlider_.setSliderStyle (Slider::Rotary);
    feedbackSlider_.addListener (this);
    feedbackSlider_.setRange (0.0, 0.995, 0.005);
    
    addAndMakeVisible (&dryMixSlider_);
    dryMixSlider_.setSliderStyle (Slider::Rotary);
    dryMixSlider_.addListener (this);
    dryMixSlider_.setRange (0.0, 1.0, 0.01);
    
    addAndMakeVisible (&wetMixSlider_);
    wetMixSlider_.setSliderStyle (Slider::Rotary);
    wetMixSlider_.addListener (this);
    wetMixSlider_.setRange (0.0, 1.0, 0.01);
    
    delayLengthLabel_.attachToComponent(&delayLengthSlider_, false);
    delayLengthLabel_.setFont(Font (11.0f));
    
    feedbackLabel_.attachToComponent(&feedbackSlider_, false);
    feedbackLabel_.setFont(Font (11.0f));
    
    dryMixLabel_.attachToComponent(&dryMixSlider_, false);
    dryMixLabel_.setFont(Font (11.0f));
    
    wetMixLabel_.attachToComponent(&wetMixSlider_, false);
    wetMixLabel_.setFont(Font (11.0f));
    
    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible(resizer_ = new ResizableCornerComponent (this, &resizeLimits_));
    resizeLimits_.setSizeLimits(370, 140, 500, 300);
    
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(ownerFilter->lastUIWidth_,
            ownerFilter->lastUIHeight_);
    
    startTimer(50);
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
}

//==============================================================================
void DelayAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
}

void DelayAudioProcessorEditor::resized()
{
    delayLengthSlider_.setBounds (20, 20, 150, 40);
    feedbackSlider_.setBounds (200, 20, 150, 40);
    dryMixSlider_.setBounds(20, 80, 150, 40);
    wetMixSlider_.setBounds(200, 80, 150, 40);
    
    resizer_->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
    
    getProcessor()->lastUIWidth_ = getWidth();
    getProcessor()->lastUIHeight_ = getHeight();
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void DelayAudioProcessorEditor::timerCallback()
{
    DelayAudioProcessor* ourProcessor = getProcessor();
    
    delayLengthSlider_.setValue(ourProcessor->delayLength_, dontSendNotification);
    feedbackSlider_.setValue(ourProcessor->feedback_, dontSendNotification);
    dryMixSlider_.setValue(ourProcessor->dryMix_, dontSendNotification);
    wetMixSlider_.setValue(ourProcessor->wetMix_, dontSendNotification);
}

// This is our Slider::Listener callback, when the user drags a slider.
void DelayAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
    // by the host, rather than just modifying them directly, otherwise the host won't know
    // that they've changed.
    
    if (slider == &delayLengthSlider_)
    {
        getProcessor()->setParameterNotifyingHost (DelayAudioProcessor::kDelayLengthParam,
                                                   (float)delayLengthSlider_.getValue());
    }
    else if (slider == &feedbackSlider_)
    {
        getProcessor()->setParameterNotifyingHost (DelayAudioProcessor::kFeedbackParam,
                                                   (float)feedbackSlider_.getValue());
    }
    else if (slider == &dryMixSlider_)
    {
        getProcessor()->setParameterNotifyingHost (DelayAudioProcessor::kDryMixParam,
                                                   (float)dryMixSlider_.getValue());
    }
    else if (slider == &wetMixSlider_)
    {
        getProcessor()->setParameterNotifyingHost (DelayAudioProcessor::kWetMixParam,
                                                   (float)wetMixSlider_.getValue());
    }
}