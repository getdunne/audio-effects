/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Chorus: chorus effect based on time-varying delays
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
ChorusAudioProcessorEditor::ChorusAudioProcessorEditor (ChorusAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      delayLabel_("", "Min. Delay (sec):"),
      sweepWidthLabel_("", "Sweep Width (sec.):"),
      depthLabel_("", "Depth:"),
      frequencyLabel_("", "LFO Frequency:"),
      waveformLabel_("", "LFO Waveform:"),
      interpolationLabel_("", "Interpolation Type:"),
      numVoicesLabel_("", "Number of Voices:")
{

    // Set up the sliders
    addAndMakeVisible (&delaySlider_);
    delaySlider_.setSliderStyle (Slider::Rotary);
    delaySlider_.addListener (this);
    delaySlider_.setRange (.01, ChorusAudioProcessor::kMaximumDelay, 0.001);
    
    addAndMakeVisible (&sweepWidthSlider_);
    sweepWidthSlider_.setSliderStyle (Slider::Rotary);
    sweepWidthSlider_.addListener (this);
    sweepWidthSlider_.setRange (.01, ChorusAudioProcessor::kMaximumSweepWidth, 0.001);
    
    addAndMakeVisible (&depthSlider_);
    depthSlider_.setSliderStyle (Slider::Rotary);
    depthSlider_.addListener (this);
    depthSlider_.setRange (0.0, 1.0, 0.01);
    
    addAndMakeVisible (&frequencySlider_);
    frequencySlider_.setSliderStyle (Slider::Rotary);
    frequencySlider_.addListener (this);
    frequencySlider_.setRange (0.05, 2.0, 0.025);
    
    addAndMakeVisible(&waveformComboBox_);
    waveformComboBox_.setEditableText(false);
    waveformComboBox_.setJustificationType(Justification::left);
    waveformComboBox_.addItem("Sine", ChorusAudioProcessor::kWaveformSine);
    waveformComboBox_.addItem("Triangle", ChorusAudioProcessor::kWaveformTriangle);
    waveformComboBox_.addItem("Square", ChorusAudioProcessor::kWaveformSquare);
    waveformComboBox_.addItem("Sawtooth", ChorusAudioProcessor::kWaveformSawtooth);
    waveformComboBox_.addListener(this);
    
    addAndMakeVisible(&interpolationComboBox_);
    interpolationComboBox_.setEditableText(false);
    interpolationComboBox_.setJustificationType(Justification::left);
    interpolationComboBox_.addItem("None", ChorusAudioProcessor::kInterpolationNearestNeighbour);
    interpolationComboBox_.addItem("Linear", ChorusAudioProcessor::kInterpolationLinear);
    interpolationComboBox_.addItem("Cubic", ChorusAudioProcessor::kInterpolationCubic);
    interpolationComboBox_.addListener(this);
    
    addAndMakeVisible(&numVoicesComboBox_);
    numVoicesComboBox_.setEditableText(false);
    numVoicesComboBox_.setJustificationType(Justification::left);
    numVoicesComboBox_.addItem("2", 2);
    numVoicesComboBox_.addItem("3", 3);
    numVoicesComboBox_.addItem("4", 4);
    numVoicesComboBox_.addItem("5", 5);
    numVoicesComboBox_.addListener(this);
    
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

    frequencyLabel_.attachToComponent(&frequencySlider_, false);
    frequencyLabel_.setFont(Font (11.0f));
    
    waveformLabel_.attachToComponent(&waveformComboBox_, false);
    waveformLabel_.setFont(Font (11.0f));
    
    interpolationLabel_.attachToComponent(&interpolationComboBox_, false);
    interpolationLabel_.setFont(Font (11.0f));
    
    numVoicesLabel_.attachToComponent(&numVoicesComboBox_, false);
    numVoicesLabel_.setFont(Font (11.0f));
    
    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible(resizer_ = new ResizableCornerComponent (this, &resizeLimits_));
    resizeLimits_.setSizeLimits(550, 200, 600, 300);
    
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(ownerFilter->lastUIWidth_,
            ownerFilter->lastUIHeight_);
    
    startTimer(50);
}

ChorusAudioProcessorEditor::~ChorusAudioProcessorEditor()
{
}

//==============================================================================
void ChorusAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
}

void ChorusAudioProcessorEditor::resized()
{
    delaySlider_.setBounds (20, 20, 150, 40);
    sweepWidthSlider_.setBounds (200, 20, 150, 40);
    depthSlider_.setBounds(380, 20, 150, 40);
    frequencySlider_.setBounds(20, 80, 150, 40);
    numVoicesComboBox_.setBounds(200, 80, 150, 30);
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
void ChorusAudioProcessorEditor::timerCallback()
{
    ChorusAudioProcessor* ourProcessor = getProcessor();
    
    delaySlider_.setValue(ourProcessor->delay_, dontSendNotification);
    sweepWidthSlider_.setValue(ourProcessor->sweepWidth_, dontSendNotification);
    depthSlider_.setValue(ourProcessor->depth_, dontSendNotification);
    frequencySlider_.setValue(ourProcessor->frequency_, dontSendNotification);
    waveformComboBox_.setSelectedId(ourProcessor->waveform_, dontSendNotification);
    interpolationComboBox_.setSelectedId(ourProcessor->interpolation_, dontSendNotification);
    numVoicesComboBox_.setSelectedId(ourProcessor->numVoices_, dontSendNotification);
    stereoToggleButton_.setToggleState((ourProcessor->stereo_ != 0), dontSendNotification);
}

// This is our Slider::Listener callback, when the user drags a slider.
void ChorusAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
    // by the host, rather than just modifying them directly, otherwise the host won't know
    // that they've changed.
    
    if (slider == &delaySlider_)
    {
        getProcessor()->setParameterNotifyingHost (ChorusAudioProcessor::kDelayParam,
                                                   (float)delaySlider_.getValue());
    }
    else if (slider == &sweepWidthSlider_)
    {
        getProcessor()->setParameterNotifyingHost (ChorusAudioProcessor::kSweepWidthParam,
                                                   (float)sweepWidthSlider_.getValue());
    }
    else if (slider == &depthSlider_)
    {
        getProcessor()->setParameterNotifyingHost (ChorusAudioProcessor::kDepthParam,
                                                   (float)depthSlider_.getValue());
    }
    else if (slider == &frequencySlider_)
    {
        getProcessor()->setParameterNotifyingHost (ChorusAudioProcessor::kFrequencyParam,
                                                   (float)frequencySlider_.getValue());
    }
}

// Similar callback to sliderValueChanged for ComboBox updates
void ChorusAudioProcessorEditor::comboBoxChanged (ComboBox *comboBox)
{
    if(comboBox == &waveformComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (ChorusAudioProcessor::kWaveformParam,
                                                   (float)waveformComboBox_.getSelectedId());
    }
    else if(comboBox == &interpolationComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (ChorusAudioProcessor::kInterpolationParam,
                                                   (float)interpolationComboBox_.getSelectedId());
    }
    else if(comboBox == &numVoicesComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (ChorusAudioProcessor::kNumVoicesParam,
                                                   (float)numVoicesComboBox_.getSelectedId());
    }
}

// Callback for toggle button
void ChorusAudioProcessorEditor::buttonClicked (Button *button)
{
    if(button == &stereoToggleButton_)
    {
        if(button->getToggleState())
        {
            getProcessor()->setParameterNotifyingHost (ChorusAudioProcessor::kStereoParam, 1.0);
            
            // Stereo chorus only makes sense with 3 or more voices (1 input, 2 delayed voices)
            if(numVoicesComboBox_.getSelectedId() == 2)
            {
                numVoicesComboBox_.setSelectedId(3);
                getProcessor()->setParameterNotifyingHost (ChorusAudioProcessor::kNumVoicesParam, 3);
            }
            numVoicesComboBox_.setItemEnabled(2, false);
        }
        else
        {
            getProcessor()->setParameterNotifyingHost (ChorusAudioProcessor::kStereoParam, 0.0);
            numVoicesComboBox_.setItemEnabled(2, true);
        }
    }
}