/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  PVOC Passthrough: phase vocoder structure which passes input
  to output without performing any processing
 
  See textbook Chapter 8: The Phase Vocoder
 
  Code by Andrew McPherson, Brecht De Man and Joshua Reiss
 
  This code requires the fftw library version 3 to compile:
  http://fftw.org
 
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
PVOCPassthroughAudioProcessorEditor::PVOCPassthroughAudioProcessorEditor (PVOCPassthroughAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      fftSizeLabel_("", "FFT Size:"),
      hopSizeLabel_("", "Hop Size:"),
      windowTypeLabel_("", "Window Type:")
{
    // This is where our plugin's editor size is set.
    // setSize(170, 80);
    
    // Set up the selection boxes
    
    addAndMakeVisible(&fftSizeComboBox_);
    fftSizeComboBox_.setEditableText(false);
    fftSizeComboBox_.setJustificationType(Justification::left);
    fftSizeComboBox_.addItem("64", 64);
    fftSizeComboBox_.addItem("128", 128);
    fftSizeComboBox_.addItem("256", 256);
    fftSizeComboBox_.addItem("512", 512);
    fftSizeComboBox_.addItem("1024", 1024);
    fftSizeComboBox_.addItem("2048", 2048);
    fftSizeComboBox_.addListener(this);
    
    addAndMakeVisible(&hopSizeComboBox_);
    hopSizeComboBox_.setEditableText(false);
    hopSizeComboBox_.setJustificationType(Justification::left);
    hopSizeComboBox_.addItem("1 Window", PVOCPassthroughAudioProcessor::kHopSize1Window);
    hopSizeComboBox_.addItem("1/2 Window", PVOCPassthroughAudioProcessor::kHopSize1_2Window);
    hopSizeComboBox_.addItem("1/4 Window", PVOCPassthroughAudioProcessor::kHopSize1_4Window);
    hopSizeComboBox_.addItem("1/8 Window", PVOCPassthroughAudioProcessor::kHopSize1_8Window);
    hopSizeComboBox_.addListener(this);
    
    addAndMakeVisible(&windowTypeComboBox_);
    windowTypeComboBox_.setEditableText(false);
    windowTypeComboBox_.setJustificationType(Justification::left);
    windowTypeComboBox_.addItem("Rectangular", PVOCPassthroughAudioProcessor::kWindowRectangular);
    windowTypeComboBox_.addItem("Bartlett", PVOCPassthroughAudioProcessor::kWindowBartlett);
    windowTypeComboBox_.addItem("Hann", PVOCPassthroughAudioProcessor::kWindowHann);
    windowTypeComboBox_.addItem("Hamming", PVOCPassthroughAudioProcessor::kWindowHamming);
    windowTypeComboBox_.addListener(this);
    
    fftSizeLabel_.attachToComponent(&fftSizeComboBox_, false);
    fftSizeLabel_.setFont(Font (11.0f));
    
    hopSizeLabel_.attachToComponent(&hopSizeComboBox_, false);
    hopSizeLabel_.setFont(Font (11.0f));
    
    windowTypeLabel_.attachToComponent(&windowTypeComboBox_, false);
    windowTypeLabel_.setFont(Font (11.0f));
    
    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible(resizer_ = new ResizableCornerComponent (this, &resizeLimits_));
    resizeLimits_.setSizeLimits(370, 120, 400, 160);
    
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(ownerFilter->lastUIWidth_,
            ownerFilter->lastUIHeight_);
    
    startTimer(50);
}

PVOCPassthroughAudioProcessorEditor::~PVOCPassthroughAudioProcessorEditor()
{
}

//==============================================================================
void PVOCPassthroughAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
}

void PVOCPassthroughAudioProcessorEditor::resized()
{
    fftSizeComboBox_.setBounds(20, 20, 150, 30);
    hopSizeComboBox_.setBounds(200, 20, 150, 30);
    windowTypeComboBox_.setBounds(20, 70, 150, 30);
    
    resizer_->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
    
    getProcessor()->lastUIWidth_ = getWidth();
    getProcessor()->lastUIHeight_ = getHeight();
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void PVOCPassthroughAudioProcessorEditor::timerCallback()
{
    PVOCPassthroughAudioProcessor* ourProcessor = getProcessor();
    
    fftSizeComboBox_.setSelectedId(ourProcessor->fftSelectedSize_, dontSendNotification);
    hopSizeComboBox_.setSelectedId(ourProcessor->hopSelectedSize_, dontSendNotification);
    windowTypeComboBox_.setSelectedId(ourProcessor->windowType_, dontSendNotification);
}

// This is our Slider::Listener callback, when the user drags a slider.
void PVOCPassthroughAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
}

// Similar callback to sliderValueChanged for ComboBox updates
void PVOCPassthroughAudioProcessorEditor::comboBoxChanged (ComboBox *comboBox)
{
    if(comboBox == &fftSizeComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (PVOCPassthroughAudioProcessor::kFFTSizeParam,
                                                   (float)fftSizeComboBox_.getSelectedId());
    }
    else if(comboBox == &hopSizeComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (PVOCPassthroughAudioProcessor::kHopSizeParam,
                                                   (float)hopSizeComboBox_.getSelectedId());
    }
    else if(comboBox == &windowTypeComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (PVOCPassthroughAudioProcessor::kWindowTypeParam,
                                                   (float)windowTypeComboBox_.getSelectedId());
    }
}
