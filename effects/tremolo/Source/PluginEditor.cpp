/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Tremolo: amplitude modulation using a low-frequency oscillator
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
TremoloAudioProcessorEditor::TremoloAudioProcessorEditor (TremoloAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      depthLabel_("", "Depth:"),
      frequencyLabel_("", "Frequency:"),
      waveformLabel_("", "Waveform:")
{
    // Set up the sliders
    addAndMakeVisible (&depthSlider_);
    depthSlider_.setSliderStyle (Slider::Rotary);
    depthSlider_.addListener (this);
    depthSlider_.setRange (0.0, 1.0, 0.01);

    addAndMakeVisible (&frequencySlider_);
    frequencySlider_.setSliderStyle (Slider::Rotary);
    frequencySlider_.addListener (this);
    frequencySlider_.setRange (0.2, 20.0, 0.05);
    
    addAndMakeVisible(&waveformComboBox_);
    waveformComboBox_.setEditableText(false);
    waveformComboBox_.setJustificationType(Justification::left);
    waveformComboBox_.addItem("Sine", TremoloAudioProcessor::kWaveformSine);
    waveformComboBox_.addItem("Triangle", TremoloAudioProcessor::kWaveformTriangle);
    waveformComboBox_.addItem("Square", TremoloAudioProcessor::kWaveformSquare);
    waveformComboBox_.addItem("Square (with sloped edges)", TremoloAudioProcessor::kWaveformSquareSlopedEdges);
    waveformComboBox_.addListener(this);

    depthLabel_.attachToComponent(&depthSlider_, false);
    depthLabel_.setFont(Font (11.0f));
    
    frequencyLabel_.attachToComponent(&frequencySlider_, false);
    frequencyLabel_.setFont(Font (11.0f));
    
    waveformLabel_.attachToComponent(&waveformComboBox_, false);
    waveformLabel_.setFont(Font (11.0f));
    
    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible(resizer_ = new ResizableCornerComponent (this, &resizeLimits_));
    resizeLimits_.setSizeLimits(370, 140, 600, 300);
    
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(ownerFilter->lastUIWidth_,
            ownerFilter->lastUIHeight_);
    
    startTimer(50);
}

TremoloAudioProcessorEditor::~TremoloAudioProcessorEditor()
{
}

//==============================================================================
void TremoloAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
}

void TremoloAudioProcessorEditor::resized()
{
    frequencySlider_.setBounds(20, 20, 150, 40);
    depthSlider_.setBounds (200, 20, 150, 40);
    waveformComboBox_.setBounds (20, 80, 200, 30);
    
    resizer_->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
    
    getProcessor()->lastUIWidth_ = getWidth();
    getProcessor()->lastUIHeight_ = getHeight();
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void TremoloAudioProcessorEditor::timerCallback()
{
    TremoloAudioProcessor* ourProcessor = getProcessor();
    
    depthSlider_.setValue(ourProcessor->depth_, dontSendNotification);
    frequencySlider_.setValue(ourProcessor->frequency_, dontSendNotification);
    waveformComboBox_.setSelectedId(ourProcessor->waveform_, dontSendNotification);
}

// This is our Slider::Listener callback, when the user drags a slider.
void TremoloAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
    // by the host, rather than just modifying them directly, otherwise the host won't know
    // that they've changed.
    
    if (slider == &depthSlider_)
    {
        getProcessor()->setParameterNotifyingHost (TremoloAudioProcessor::kDepthParam,
                                                   (float)depthSlider_.getValue());
    }
    else if (slider == &frequencySlider_)
    {
        getProcessor()->setParameterNotifyingHost (TremoloAudioProcessor::kFrequencyParam,
                                                   (float)frequencySlider_.getValue());
    }
}

// Similar callback to sliderValueChanged for ComboBox updates
void TremoloAudioProcessorEditor::comboBoxChanged (ComboBox *comboBox)
{
    if(comboBox == &waveformComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (TremoloAudioProcessor::kWaveformParam,
                                                   (float)waveformComboBox_.getSelectedId());
    }
}
