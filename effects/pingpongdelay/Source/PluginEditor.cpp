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

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
PingPongDelayAudioProcessorEditor::PingPongDelayAudioProcessorEditor (PingPongDelayAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      delayLengthLeftLabel_("", "L-R Delay (sec):"),
      delayLengthRightLabel_("", "R-L Delay (sec):"),
      feedbackLabel_("", "Feedback:"),
      wetMixLabel_("", "Delayed Mix Level:")
{

    // Set up the sliders
    addAndMakeVisible (&delayLengthLeftSlider_);
    delayLengthLeftSlider_.setSliderStyle (Slider::Rotary);
    delayLengthLeftSlider_.addListener (this);
    delayLengthLeftSlider_.setRange (0.01, 2.0, 0.01);

    addAndMakeVisible (&delayLengthRightSlider_);
    delayLengthRightSlider_.setSliderStyle (Slider::Rotary);
    delayLengthRightSlider_.addListener (this);
    delayLengthRightSlider_.setRange (0.01, 2.0, 0.01);
    
    addAndMakeVisible (&feedbackSlider_);
    feedbackSlider_.setSliderStyle (Slider::Rotary);
    feedbackSlider_.addListener (this);
    feedbackSlider_.setRange (0.0, 0.995, 0.005);
 
    addAndMakeVisible (&wetMixSlider_);
    wetMixSlider_.setSliderStyle (Slider::Rotary);
    wetMixSlider_.addListener (this);
    wetMixSlider_.setRange (0.0, 1.0, 0.01);
    
    addAndMakeVisible(&linkDelaysButton_);
    linkDelaysButton_.setName("Link Delays");
    linkDelaysButton_.setButtonText("Link Delays");
    linkDelaysButton_.setToggleState(true, dontSendNotification);
    linkDelaysButton_.addListener(this);
    
    addAndMakeVisible(&reverseChannelsButton_);
    reverseChannelsButton_.setName("Reverse Output Channels");
    reverseChannelsButton_.setButtonText("Reverse Output Channels");
    reverseChannelsButton_.setToggleState(false, dontSendNotification);
    reverseChannelsButton_.addListener(this);
    
    linkDelays_ = true;
    
    delayLengthLeftLabel_.attachToComponent(&delayLengthLeftSlider_, false);
    delayLengthLeftLabel_.setFont(Font (11.0f));

    delayLengthRightLabel_.attachToComponent(&delayLengthRightSlider_, false);
    delayLengthRightLabel_.setFont(Font (11.0f));
    
    feedbackLabel_.attachToComponent(&feedbackSlider_, false);
    feedbackLabel_.setFont(Font (11.0f));
    
    wetMixLabel_.attachToComponent(&wetMixSlider_, false);
    wetMixLabel_.setFont(Font (11.0f));
    
    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible(resizer_ = new ResizableCornerComponent (this, &resizeLimits_));
    resizeLimits_.setSizeLimits(500, 140, 500, 300);
    
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(ownerFilter->lastUIWidth_,
            ownerFilter->lastUIHeight_);
    
    startTimer(50);
}

PingPongDelayAudioProcessorEditor::~PingPongDelayAudioProcessorEditor()
{
}

//==============================================================================
void PingPongDelayAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
}

void PingPongDelayAudioProcessorEditor::resized()
{
    delayLengthLeftSlider_.setBounds (20, 20, 150, 40);
    delayLengthRightSlider_.setBounds (200, 20, 150, 40);
    feedbackSlider_.setBounds (20, 80, 150, 40);
    wetMixSlider_.setBounds(200, 80, 150, 40);
    linkDelaysButton_.setBounds(380, 20, 100, 40);
    linkDelaysButton_.changeWidthToFitText();
    reverseChannelsButton_.setBounds(380, 80, 100, 40);
    reverseChannelsButton_.changeWidthToFitText();
    
    resizer_->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
    
    getProcessor()->lastUIWidth_ = getWidth();
    getProcessor()->lastUIHeight_ = getHeight();
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void PingPongDelayAudioProcessorEditor::timerCallback()
{
    PingPongDelayAudioProcessor* ourProcessor = getProcessor();
    
    delayLengthLeftSlider_.setValue(ourProcessor->delayLengthLeft_, dontSendNotification);
    delayLengthRightSlider_.setValue(ourProcessor->delayLengthRight_, dontSendNotification);
    feedbackSlider_.setValue(ourProcessor->feedback_, dontSendNotification);
    wetMixSlider_.setValue(ourProcessor->wetMix_, dontSendNotification);
    reverseChannelsButton_.setToggleState(ourProcessor->reverseChannels_, dontSendNotification);
}

// This is our Slider::Listener callback, when the user drags a slider.
void PingPongDelayAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
    // by the host, rather than just modifying them directly, otherwise the host won't know
    // that they've changed.
    
    if (slider == &delayLengthLeftSlider_)
    {
        getProcessor()->setParameterNotifyingHost (PingPongDelayAudioProcessor::kDelayLengthLeftParam,
                                                   (float)delayLengthLeftSlider_.getValue());
        if(linkDelays_)
        {
            // Set the other slider to match this value
            getProcessor()->setParameterNotifyingHost (PingPongDelayAudioProcessor::kDelayLengthRightParam,
                                                       (float)delayLengthLeftSlider_.getValue());
            delayLengthRightSlider_.setValue(delayLengthLeftSlider_.getValue(), dontSendNotification);
        }
    }
    else if (slider == &delayLengthRightSlider_)
    {
        getProcessor()->setParameterNotifyingHost (PingPongDelayAudioProcessor::kDelayLengthRightParam,
                                                   (float)delayLengthRightSlider_.getValue());

        if(linkDelays_)
        {
            // Set the other slider to match this value
            getProcessor()->setParameterNotifyingHost (PingPongDelayAudioProcessor::kDelayLengthLeftParam,
                                                       (float)delayLengthRightSlider_.getValue());
            delayLengthLeftSlider_.setValue(delayLengthRightSlider_.getValue(), dontSendNotification);
        }
    }
    else if (slider == &feedbackSlider_)
    {
        getProcessor()->setParameterNotifyingHost (PingPongDelayAudioProcessor::kFeedbackParam,
                                                   (float)feedbackSlider_.getValue());
    }
    else if (slider == &wetMixSlider_)
    {
        getProcessor()->setParameterNotifyingHost (PingPongDelayAudioProcessor::kWetMixParam,
                                                   (float)wetMixSlider_.getValue());
    }
}

// Callback for toggle button
void PingPongDelayAudioProcessorEditor::buttonClicked (Button *button)
{
    if(button == &linkDelaysButton_)
    {
        linkDelays_ = button->getToggleState();
        
        if(linkDelays_)
        {
            // Keep L and R delays equal, setting them to be if they aren't already
            PingPongDelayAudioProcessor* ourProcessor = getProcessor();
            
            delayLengthRightSlider_.setValue(ourProcessor->delayLengthLeft_, dontSendNotification);
            ourProcessor->setParameterNotifyingHost(PingPongDelayAudioProcessor::kDelayLengthRightParam,
                                                    ourProcessor->delayLengthLeft_);
        }
    }
    else if(button == &reverseChannelsButton_)
    {
        bool reverse = button->getToggleState();
        
        if(reverse)
            getProcessor()->setParameterNotifyingHost(PingPongDelayAudioProcessor::kReverseChannelsParam, 1.0);
        else
            getProcessor()->setParameterNotifyingHost(PingPongDelayAudioProcessor::kReverseChannelsParam, 0.0);
    }
}