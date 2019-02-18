/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Ring Modulator: modulation using a carrier oscillator
  See textbook Chapter 5: Amplitude Modulation
 
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
RingModulatorAudioProcessorEditor::RingModulatorAudioProcessorEditor (RingModulatorAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      sweepWidthLabel_("", "LFO Sweep Width (Hz):"),
      lfoFrequencyLabel_("", "LFO Frequency:"),
      carrierFrequencyLabel_("", "Carrier Frequency:"),
      waveformLabel_("", "LFO Waveform:")
{
    // Set up the sliders
    addAndMakeVisible (&carrierFrequencySlider_);
    carrierFrequencySlider_.setSliderStyle (Slider::Rotary);
    carrierFrequencySlider_.addListener (this);
    carrierFrequencySlider_.setRange (1.0, 1000.0, 1.0);
    
    addAndMakeVisible (&sweepWidthSlider_);
    sweepWidthSlider_.setSliderStyle (Slider::Rotary);
    sweepWidthSlider_.addListener (this);
    sweepWidthSlider_.setRange (0.0, 100.0, 0.2);

    addAndMakeVisible (&lfoFrequencySlider_);
    lfoFrequencySlider_.setSliderStyle (Slider::Rotary);
    lfoFrequencySlider_.addListener (this);
    lfoFrequencySlider_.setRange (0.1, 10.0, 0.05);
    
    addAndMakeVisible(&waveformComboBox_);
    waveformComboBox_.setEditableText(false);
    waveformComboBox_.setJustificationType(Justification::left);
    waveformComboBox_.addItem("Sine", RingModulatorAudioProcessor::kWaveformSine);
    waveformComboBox_.addItem("Triangle", RingModulatorAudioProcessor::kWaveformTriangle);
    waveformComboBox_.addItem("Square", RingModulatorAudioProcessor::kWaveformSquare);
    waveformComboBox_.addItem("Sawtooth (rising)", RingModulatorAudioProcessor::kWaveformSawtooth);
    waveformComboBox_.addItem("Sawtooth (falling)", RingModulatorAudioProcessor::kWaveformInverseSawtooth);
    waveformComboBox_.addListener(this);

    carrierFrequencyLabel_.attachToComponent(&carrierFrequencySlider_, false);
    carrierFrequencyLabel_.setFont(Font (11.0f));
    
    sweepWidthLabel_.attachToComponent(&sweepWidthSlider_, false);
    sweepWidthLabel_.setFont(Font (11.0f));
    
    lfoFrequencyLabel_.attachToComponent(&lfoFrequencySlider_, false);
    lfoFrequencyLabel_.setFont(Font (11.0f));
    
    waveformLabel_.attachToComponent(&waveformComboBox_, false);
    waveformLabel_.setFont(Font (11.0f));
    
    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible(resizer_ = new ResizableCornerComponent (this, &resizeLimits_));
    resizeLimits_.setSizeLimits(370, 160, 600, 300);
    
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(ownerFilter->lastUIWidth_,
            ownerFilter->lastUIHeight_);
    
    startTimer(50);
}

RingModulatorAudioProcessorEditor::~RingModulatorAudioProcessorEditor()
{
}

//==============================================================================
void RingModulatorAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
}

void RingModulatorAudioProcessorEditor::resized()
{
    carrierFrequencySlider_.setBounds(20, 20, 150, 40);
    lfoFrequencySlider_.setBounds(200, 20, 150, 40);
    sweepWidthSlider_.setBounds (20, 80, 150, 40);
    waveformComboBox_.setBounds (200, 80, 150, 30);
    
    resizer_->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
    
    getProcessor()->lastUIWidth_ = getWidth();
    getProcessor()->lastUIHeight_ = getHeight();
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void RingModulatorAudioProcessorEditor::timerCallback()
{
    RingModulatorAudioProcessor* ourProcessor = getProcessor();
    
    carrierFrequencySlider_.setValue(ourProcessor->carrierFrequency_, dontSendNotification);
    sweepWidthSlider_.setValue(ourProcessor->sweepWidth_, dontSendNotification);
    lfoFrequencySlider_.setValue(ourProcessor->lfoFrequency_, dontSendNotification);
    waveformComboBox_.setSelectedId(ourProcessor->waveform_, dontSendNotification);
}

// This is our Slider::Listener callback, when the user drags a slider.
void RingModulatorAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
    // by the host, rather than just modifying them directly, otherwise the host won't know
    // that they've changed.
    
    if (slider == &carrierFrequencySlider_)
    {
        getProcessor()->setParameterNotifyingHost (RingModulatorAudioProcessor::kCarrierFrequencyParam,
                                                   (float)carrierFrequencySlider_.getValue());
    }
    else if (slider == &sweepWidthSlider_)
    {
        getProcessor()->setParameterNotifyingHost (RingModulatorAudioProcessor::kSweepWidthParam,
                                                   (float)sweepWidthSlider_.getValue());
    }
    else if (slider == &lfoFrequencySlider_)
    {
        getProcessor()->setParameterNotifyingHost (RingModulatorAudioProcessor::kLFOFrequencyParam,
                                                   (float)lfoFrequencySlider_.getValue());
    }
}

// Similar callback to sliderValueChanged for ComboBox updates
void RingModulatorAudioProcessorEditor::comboBoxChanged (ComboBox *comboBox)
{
    if(comboBox == &waveformComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (RingModulatorAudioProcessor::kWaveformParam,
                                                   (float)waveformComboBox_.getSelectedId());
    }
}
