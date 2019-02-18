/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Parametric EQ: parametric equaliser adjusting frequency, Q and gain
  See textbook Chapter 4: Filter Effects
 
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
#include <cstring>

#ifdef _MSC_VER
#define snprintf _snprintf_s //support for pre-2014 versions of Visual Studio
#endif // _MSC_VER

//==============================================================================
ParametricEQAudioProcessorEditor::ParametricEQAudioProcessorEditor (ParametricEQAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      centreFrequencyLabel_("", "Centre frequency (Hz):"),
      qLabel_("", "Q:"),
      gainDecibelsLabel_("", "Gain (dB):"),
      bandwidthLabel_("", "Bandwidth:")
{
    // Set up the sliders

    addAndMakeVisible(&centreFrequencySlider_);
    centreFrequencySlider_.setSliderStyle(Slider::Rotary);
    centreFrequencySlider_.addListener(this);
    centreFrequencySlider_.setRange(10.0, 20000.0, 0.1);
    
    // Make the centre frequency slider (approximately) logarithmic for a
    // more natural feel
    centreFrequencySlider_.setSkewFactorFromMidPoint(sqrt(10.0 * 20000.0));
    
    addAndMakeVisible(&qSlider_);
    qSlider_.setSliderStyle(Slider::Rotary);
    qSlider_.addListener(this);
    qSlider_.setRange(0.1, 20.0, 0.01);
    
    addAndMakeVisible(&gainDecibelsSlider_);
    gainDecibelsSlider_.setSliderStyle(Slider::Rotary);
    gainDecibelsSlider_.addListener(this);
    gainDecibelsSlider_.setRange(-12.0, 12.0, 0.1);
    
    // This label is informational and exists apart from other controls
    // The other labels are attached to sliders and combo boxes
    addAndMakeVisible(&bandwidthLabel_);
    bandwidthLabel_.setFont(Font (12.0f));
    
    centreFrequencyLabel_.attachToComponent(&centreFrequencySlider_, false);
    centreFrequencyLabel_.setFont(Font (11.0f));
    
    qLabel_.attachToComponent(&qSlider_, false);
    qLabel_.setFont(Font (11.0f));
    
    gainDecibelsLabel_.attachToComponent(&gainDecibelsSlider_, false);
    gainDecibelsLabel_.setFont(Font (11.0f));
    
    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible(resizer_ = new ResizableCornerComponent (this, &resizeLimits_));
    resizeLimits_.setSizeLimits(550, 100, 550, 160);
    
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(ownerFilter->lastUIWidth_,
            ownerFilter->lastUIHeight_);
    
    startTimer(50);
}

ParametricEQAudioProcessorEditor::~ParametricEQAudioProcessorEditor()
{
}

//==============================================================================
void ParametricEQAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
}

void ParametricEQAudioProcessorEditor::resized()
{
    centreFrequencySlider_.setBounds(20, 20, 150, 40);
    qSlider_.setBounds(200, 20, 150, 40);
    gainDecibelsSlider_.setBounds(380, 20, 150, 40);
    bandwidthLabel_.setBounds(20, 70, 350, 20);
    
    resizer_->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
    
    getProcessor()->lastUIWidth_ = getWidth();
    getProcessor()->lastUIHeight_ = getHeight();
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void ParametricEQAudioProcessorEditor::timerCallback()
{
    ParametricEQAudioProcessor* ourProcessor = getProcessor();
    
    centreFrequencySlider_.setValue(ourProcessor->centreFrequency_, dontSendNotification);
    qSlider_.setValue(ourProcessor->q_, dontSendNotification);
    gainDecibelsSlider_.setValue(ourProcessor->gainDecibels_, dontSendNotification);
    updateBandwidthLabel();
}

// This is our Slider::Listener callback, when the user drags a slider.
void ParametricEQAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
    // by the host, rather than just modifying them directly, otherwise the host won't know
    // that they've changed.
    
    if (slider == &centreFrequencySlider_)
    {
        getProcessor()->setParameterNotifyingHost (ParametricEQAudioProcessor::kCentreFrequencyParam,
                                                   (float)centreFrequencySlider_.getValue());
        updateBandwidthLabel();
    }
    else if (slider == &qSlider_)
    {
        getProcessor()->setParameterNotifyingHost (ParametricEQAudioProcessor::kQParam,
                                                   (float)qSlider_.getValue());
        updateBandwidthLabel();
    }
    else if (slider == &gainDecibelsSlider_)
    {
        getProcessor()->setParameterNotifyingHost (ParametricEQAudioProcessor::kGainDecibelsParam,
                                                   (float)gainDecibelsSlider_.getValue());
    }
}

void ParametricEQAudioProcessorEditor::updateBandwidthLabel()
{
    char str[64];
    ParametricEQAudioProcessor* ourProcessor = getProcessor();
    
    snprintf(str, 64, "Bandwidth: %.1f Hz", ourProcessor->centreFrequency_ / ourProcessor->q_);
    
    bandwidthLabel_.setText(str, dontSendNotification);
}