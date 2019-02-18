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

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
FlangerAudioProcessorEditor::FlangerAudioProcessorEditor (FlangerAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      delayLabel_("", "Min. Delay (sec):"),
      sweepWidthLabel_("", "Sweep Width (sec.):"),
      depthLabel_("", "Depth:"),
      feedbackLabel_("", "Feedback:"),
      frequencyLabel_("", "LFO Frequency:"),
      waveformLabel_("", "LFO Waveform:"),
      interpolationLabel_("", "Interpolation Type:")
{

    // Set up the sliders
    addAndMakeVisible (&delaySlider_);
    delaySlider_.setSliderStyle (Slider::Rotary);
    delaySlider_.addListener (this);
    delaySlider_.setRange (0.001, FlangerAudioProcessor::kMaximumDelay, 0.0005);
    
    addAndMakeVisible (&sweepWidthSlider_);
    sweepWidthSlider_.setSliderStyle (Slider::Rotary);
    sweepWidthSlider_.addListener (this);
    sweepWidthSlider_.setRange (.001, FlangerAudioProcessor::kMaximumSweepWidth, 0.0005);
    
    addAndMakeVisible (&depthSlider_);
    depthSlider_.setSliderStyle (Slider::Rotary);
    depthSlider_.addListener (this);
    depthSlider_.setRange (0.0, 1.0, 0.01);
    
    addAndMakeVisible (&feedbackSlider_);
    feedbackSlider_.setSliderStyle (Slider::Rotary);
    feedbackSlider_.addListener (this);
    feedbackSlider_.setRange (0.0, 0.5, 0.01);
    
    addAndMakeVisible (&frequencySlider_);
    frequencySlider_.setSliderStyle (Slider::Rotary);
    frequencySlider_.addListener (this);
    frequencySlider_.setRange (0.05, 2.0, 0.025);
    
    addAndMakeVisible(&waveformComboBox_);
    waveformComboBox_.setEditableText(false);
    waveformComboBox_.setJustificationType(Justification::left);
    waveformComboBox_.addItem("Sine", FlangerAudioProcessor::kWaveformSine);
    waveformComboBox_.addItem("Triangle", FlangerAudioProcessor::kWaveformTriangle);
    waveformComboBox_.addItem("Square", FlangerAudioProcessor::kWaveformSquare);
    waveformComboBox_.addItem("Sawtooth", FlangerAudioProcessor::kWaveformSawtooth);
    waveformComboBox_.addListener(this);
    
    addAndMakeVisible(&interpolationComboBox_);
    interpolationComboBox_.setEditableText(false);
    interpolationComboBox_.setJustificationType(Justification::left);
    interpolationComboBox_.addItem("None", FlangerAudioProcessor::kInterpolationNearestNeighbour);
    interpolationComboBox_.addItem("Linear", FlangerAudioProcessor::kInterpolationLinear);
    interpolationComboBox_.addItem("Cubic", FlangerAudioProcessor::kInterpolationCubic);
    interpolationComboBox_.addListener(this);
    
    addAndMakeVisible(&stereoToggleButton_);
    stereoToggleButton_.setName("Stereo");
    stereoToggleButton_.setButtonText("Stereo");
    stereoToggleButton_.addListener(this);
    
    delayLabel_.attachToComponent(&delaySlider_, false);
    delayLabel_.setFont(Font (11.0f));
    
    sweepWidthLabel_.attachToComponent(&sweepWidthSlider_, false);
    sweepWidthLabel_.setFont(Font (11.0f));
    
    depthLabel_.attachToComponent(&depthSlider_, false);
    depthLabel_.setFont(Font (11.0f));
    
    feedbackLabel_.attachToComponent(&feedbackSlider_, false);
    feedbackLabel_.setFont(Font (11.0f));

    frequencyLabel_.attachToComponent(&frequencySlider_, false);
    frequencyLabel_.setFont(Font (11.0f));
    
    waveformLabel_.attachToComponent(&waveformComboBox_, false);
    waveformLabel_.setFont(Font (11.0f));
    
    interpolationLabel_.attachToComponent(&interpolationComboBox_, false);
    interpolationLabel_.setFont(Font (11.0f));
    
    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible(resizer_ = new ResizableCornerComponent (this, &resizeLimits_));
    resizeLimits_.setSizeLimits(550, 200, 600, 300);
    
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(ownerFilter->lastUIWidth_,
            ownerFilter->lastUIHeight_);
    
    startTimer(50);
}

FlangerAudioProcessorEditor::~FlangerAudioProcessorEditor()
{
}

//==============================================================================
void FlangerAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
}

void FlangerAudioProcessorEditor::resized()
{
    delaySlider_.setBounds (20, 20, 150, 40);
    sweepWidthSlider_.setBounds (200, 20, 150, 40);
    depthSlider_.setBounds(380, 20, 150, 40);
    feedbackSlider_.setBounds(20, 80, 150, 40);
    frequencySlider_.setBounds(200, 80, 150, 40);
    waveformComboBox_.setBounds(20, 140, 200, 30);
    interpolationComboBox_.setBounds(250, 140, 200, 30);
    stereoToggleButton_.setBounds(380, 80, 150, 40);
    stereoToggleButton_.changeWidthToFitText();
    
    resizer_->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
    
    getProcessor()->lastUIWidth_ = getWidth();
    getProcessor()->lastUIHeight_ = getHeight();
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void FlangerAudioProcessorEditor::timerCallback()
{
    FlangerAudioProcessor* ourProcessor = getProcessor();
    
    delaySlider_.setValue(ourProcessor->delay_, dontSendNotification);
    sweepWidthSlider_.setValue(ourProcessor->sweepWidth_, dontSendNotification);
    depthSlider_.setValue(ourProcessor->depth_, dontSendNotification);
    feedbackSlider_.setValue(ourProcessor->feedback_, dontSendNotification);
    frequencySlider_.setValue(ourProcessor->frequency_, dontSendNotification);
    waveformComboBox_.setSelectedId(ourProcessor->waveform_, dontSendNotification);
    interpolationComboBox_.setSelectedId(ourProcessor->interpolation_, dontSendNotification);
    stereoToggleButton_.setToggleState((ourProcessor->stereo_ != 0), dontSendNotification);
}

// This is our Slider::Listener callback, when the user drags a slider.
void FlangerAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
    // by the host, rather than just modifying them directly, otherwise the host won't know
    // that they've changed.
    
    if (slider == &delaySlider_)
    {
        getProcessor()->setParameterNotifyingHost (FlangerAudioProcessor::kDelayParam,
                                                   (float)delaySlider_.getValue());
    }
    else if (slider == &sweepWidthSlider_)
    {
        getProcessor()->setParameterNotifyingHost (FlangerAudioProcessor::kSweepWidthParam,
                                                   (float)sweepWidthSlider_.getValue());
    }
    else if (slider == &depthSlider_)
    {
        getProcessor()->setParameterNotifyingHost (FlangerAudioProcessor::kDepthParam,
                                                   (float)depthSlider_.getValue());
    }
    else if (slider == &feedbackSlider_)
    {
        getProcessor()->setParameterNotifyingHost (FlangerAudioProcessor::kFeedbackParam,
                                                   (float)feedbackSlider_.getValue());
    }
    else if (slider == &frequencySlider_)
    {
        getProcessor()->setParameterNotifyingHost (FlangerAudioProcessor::kFrequencyParam,
                                                   (float)frequencySlider_.getValue());
    }
}

// Similar callback to sliderValueChanged for ComboBox updates
void FlangerAudioProcessorEditor::comboBoxChanged (ComboBox *comboBox)
{
    if(comboBox == &waveformComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (FlangerAudioProcessor::kWaveformParam,
                                                   (float)waveformComboBox_.getSelectedId());
    }
    else if(comboBox == &interpolationComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (FlangerAudioProcessor::kInterpolationParam,
                                                   (float)interpolationComboBox_.getSelectedId());
    }
}

// Callback for toggle button
void FlangerAudioProcessorEditor::buttonClicked (Button *button)
{
    if(button == &stereoToggleButton_)
    {
        if(button->getToggleState())
            getProcessor()->setParameterNotifyingHost (FlangerAudioProcessor::kStereoParam, 1.0);
        else
            getProcessor()->setParameterNotifyingHost (FlangerAudioProcessor::kStereoParam, 0.0);
    }
}