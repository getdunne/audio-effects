/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Phaser: phasing effect using time-varying allpass filters
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


//==============================================================================
PhaserAudioProcessorEditor::PhaserAudioProcessorEditor (PhaserAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      baseFrequencyLabel_("", "Base Frequency (Hz):"),
      sweepWidthLabel_("", "Sweep Width (Hz):"),
      depthLabel_("", "Depth:"),
      feedbackLabel_("", "Feedback:"),
      lfoFrequencyLabel_("", "LFO Frequency:"),
      waveformLabel_("", "LFO Waveform:"),
      filtersLabel_("", "Number of Filters:")
{

    // Set up the sliders
    addAndMakeVisible (&baseFrequencySlider_);
    baseFrequencySlider_.setSliderStyle (Slider::Rotary);
    baseFrequencySlider_.addListener (this);
    baseFrequencySlider_.setRange (50.0, 1000.0, 0.5);
    
    addAndMakeVisible (&sweepWidthSlider_);
    sweepWidthSlider_.setSliderStyle (Slider::Rotary);
    sweepWidthSlider_.addListener (this);
    sweepWidthSlider_.setRange (50.0, 5000.0, 0.5);
    
    addAndMakeVisible (&depthSlider_);
    depthSlider_.setSliderStyle (Slider::Rotary);
    depthSlider_.addListener (this);
    depthSlider_.setRange (0.0, 1.0, 0.01);
    
    addAndMakeVisible (&feedbackSlider_);
    feedbackSlider_.setSliderStyle (Slider::Rotary);
    feedbackSlider_.addListener (this);
    feedbackSlider_.setRange (0.0, 0.99, 0.01);
    
    addAndMakeVisible (&lfoFrequencySlider_);
    lfoFrequencySlider_.setSliderStyle (Slider::Rotary);
    lfoFrequencySlider_.addListener (this);
    lfoFrequencySlider_.setRange (0.05, 2.0, 0.025);
    
    addAndMakeVisible(&waveformComboBox_);
    waveformComboBox_.setEditableText(false);
    waveformComboBox_.setJustificationType(Justification::left);
    waveformComboBox_.addItem("Sine", PhaserAudioProcessor::kWaveformSine);
    waveformComboBox_.addItem("Triangle", PhaserAudioProcessor::kWaveformTriangle);
    waveformComboBox_.addItem("Square", PhaserAudioProcessor::kWaveformSquare);
    waveformComboBox_.addItem("Sawtooth", PhaserAudioProcessor::kWaveformSawtooth);
    waveformComboBox_.addListener(this);
    
    addAndMakeVisible(&filtersComboBox_);
    filtersComboBox_.setEditableText(false);
    filtersComboBox_.setJustificationType(Justification::left);
    filtersComboBox_.addItem("2", 2);
    filtersComboBox_.addItem("4", 4);
    filtersComboBox_.addItem("6", 6);
    filtersComboBox_.addItem("8", 8);
    filtersComboBox_.addItem("10", 10);
    filtersComboBox_.addListener(this);

    addAndMakeVisible(&stereoToggleButton_);
    stereoToggleButton_.setName("Stereo");
    stereoToggleButton_.setButtonText("Stereo");
    stereoToggleButton_.addListener(this);
    
    baseFrequencyLabel_.attachToComponent(&baseFrequencySlider_, false);
    baseFrequencyLabel_.setFont(Font (11.0f));
    
    sweepWidthLabel_.attachToComponent(&sweepWidthSlider_, false);
    sweepWidthLabel_.setFont(Font (11.0f));
    
    depthLabel_.attachToComponent(&depthSlider_, false);
    depthLabel_.setFont(Font (11.0f));
    
    feedbackLabel_.attachToComponent(&feedbackSlider_, false);
    feedbackLabel_.setFont(Font (11.0f));

    lfoFrequencyLabel_.attachToComponent(&lfoFrequencySlider_, false);
    lfoFrequencyLabel_.setFont(Font (11.0f));
    
    waveformLabel_.attachToComponent(&waveformComboBox_, false);
    waveformLabel_.setFont(Font (11.0f));
    
    filtersLabel_.attachToComponent(&filtersComboBox_, false);
    filtersLabel_.setFont(Font (11.0f));

    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible(resizer_ = new ResizableCornerComponent (this, &resizeLimits_));
    resizeLimits_.setSizeLimits(550, 200, 600, 300);
    
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(ownerFilter->lastUIWidth_,
            ownerFilter->lastUIHeight_);
    
    startTimer(50);
}

PhaserAudioProcessorEditor::~PhaserAudioProcessorEditor()
{
}

//==============================================================================
void PhaserAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
}

void PhaserAudioProcessorEditor::resized()
{
    baseFrequencySlider_.setBounds (20, 20, 150, 40);
    sweepWidthSlider_.setBounds (200, 20, 150, 40);
    depthSlider_.setBounds(380, 20, 150, 40);
    feedbackSlider_.setBounds(20, 80, 150, 40);
    lfoFrequencySlider_.setBounds(200, 80, 150, 40);
    waveformComboBox_.setBounds(20, 140, 200, 30);
    filtersComboBox_.setBounds(250, 140, 200, 30);
    stereoToggleButton_.setBounds(380, 80, 150, 40);
    stereoToggleButton_.changeWidthToFitText();
    
    resizer_->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
    
    getProcessor()->lastUIWidth_ = getWidth();
    getProcessor()->lastUIHeight_ = getHeight();
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void PhaserAudioProcessorEditor::timerCallback()
{
    PhaserAudioProcessor* ourProcessor = getProcessor();
    
    baseFrequencySlider_.setValue(ourProcessor->baseFrequency_, dontSendNotification);
    sweepWidthSlider_.setValue(ourProcessor->sweepWidth_, dontSendNotification);
    depthSlider_.setValue(ourProcessor->depth_, dontSendNotification);
    feedbackSlider_.setValue(ourProcessor->feedback_, dontSendNotification);
    lfoFrequencySlider_.setValue(ourProcessor->lfoFrequency_, dontSendNotification);
    waveformComboBox_.setSelectedId(ourProcessor->waveform_, dontSendNotification);
    filtersComboBox_.setSelectedId(ourProcessor->filtersPerChannel_, dontSendNotification);
    stereoToggleButton_.setToggleState((ourProcessor->stereo_ != 0), dontSendNotification);
}

// This is our Slider::Listener callback, when the user drags a slider.
void PhaserAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
    // by the host, rather than just modifying them directly, otherwise the host won't know
    // that they've changed.
    
    if (slider == &baseFrequencySlider_)
    {
        getProcessor()->setParameterNotifyingHost (PhaserAudioProcessor::kBaseFrequencyParam,
                                                   (float)baseFrequencySlider_.getValue());
    }
    else if (slider == &sweepWidthSlider_)
    {
        getProcessor()->setParameterNotifyingHost (PhaserAudioProcessor::kSweepWidthParam,
                                                   (float)sweepWidthSlider_.getValue());
    }
    else if (slider == &depthSlider_)
    {
        getProcessor()->setParameterNotifyingHost (PhaserAudioProcessor::kDepthParam,
                                                   (float)depthSlider_.getValue());
    }
    else if (slider == &feedbackSlider_)
    {
        getProcessor()->setParameterNotifyingHost (PhaserAudioProcessor::kFeedbackParam,
                                                   (float)feedbackSlider_.getValue());
    }
    else if (slider == &lfoFrequencySlider_)
    {
        getProcessor()->setParameterNotifyingHost (PhaserAudioProcessor::kLFOFrequencyParam,
                                                   (float)lfoFrequencySlider_.getValue());
    }
}

// Similar callback to sliderValueChanged for ComboBox updates
void PhaserAudioProcessorEditor::comboBoxChanged (ComboBox *comboBox)
{
    if(comboBox == &waveformComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (PhaserAudioProcessor::kWaveformParam,
                                                   (float)waveformComboBox_.getSelectedId());
    }
    else if(comboBox == &filtersComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (PhaserAudioProcessor::kFiltersParam,
                                                   (float)filtersComboBox_.getSelectedId());
    }
}

// Callback for toggle button
void PhaserAudioProcessorEditor::buttonClicked (Button *button)
{
    if(button == &stereoToggleButton_)
    {
        if(button->getToggleState())
            getProcessor()->setParameterNotifyingHost (PhaserAudioProcessor::kStereoParam, 1.0);
        else
            getProcessor()->setParameterNotifyingHost (PhaserAudioProcessor::kStereoParam, 0.0);
    }
}