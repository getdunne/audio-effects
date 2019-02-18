/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Vibrato: frequency modulation using delay lines
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
#include <cstring>

#ifdef _MSC_VER
#define snprintf _snprintf_s //support for pre-2014 versions of Visual Studio
#endif // _MSC_VER

//==============================================================================
VibratoAudioProcessorEditor::VibratoAudioProcessorEditor (VibratoAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      sweepWidthLabel_("", "Sweep Width (sec.):"),
      frequencyLabel_("", "LFO Frequency:"),
      waveformLabel_("", "LFO Waveform:"),
      interpolationLabel_("", "Interpolation Type:"),
      pitchShiftLabel_("", "Maximum Pitch Shift:")
{
    // Initialise variables
    oldSweepWidth_ = oldFrequency_ = -1.0;
    oldWaveform_ = -1;
    
    // Set up the sliders
    addAndMakeVisible (&sweepWidthSlider_);
    sweepWidthSlider_.setSliderStyle (Slider::Rotary);
    sweepWidthSlider_.addListener (this);
    sweepWidthSlider_.setRange (.001, VibratoAudioProcessor::kMaximumSweepWidth, 0.0005);

    addAndMakeVisible (&frequencySlider_);
    frequencySlider_.setSliderStyle (Slider::Rotary);
    frequencySlider_.addListener (this);
    frequencySlider_.setRange (0.5, 5.0, 0.025);
    
    addAndMakeVisible(&waveformComboBox_);
    waveformComboBox_.setEditableText(false);
    waveformComboBox_.setJustificationType(Justification::left);
    waveformComboBox_.addItem("Sine", VibratoAudioProcessor::kWaveformSine);
    waveformComboBox_.addItem("Triangle", VibratoAudioProcessor::kWaveformTriangle);
    waveformComboBox_.addItem("Sawtooth (rising)", VibratoAudioProcessor::kWaveformSawtooth);
    waveformComboBox_.addItem("Sawtooth (falling)", VibratoAudioProcessor::kWaveformInverseSawtooth);
    waveformComboBox_.addListener(this);
    
    addAndMakeVisible(&interpolationComboBox_);
    interpolationComboBox_.setEditableText(false);
    interpolationComboBox_.setJustificationType(Justification::left);
    interpolationComboBox_.addItem("None", VibratoAudioProcessor::kInterpolationNearestNeighbour);
    interpolationComboBox_.addItem("Linear", VibratoAudioProcessor::kInterpolationLinear);
    interpolationComboBox_.addItem("Cubic", VibratoAudioProcessor::kInterpolationCubic);
    interpolationComboBox_.addListener(this);
    
    // This label is informational and exists apart from other controls
    // The other labels are attached to sliders and combo boxes
    addAndMakeVisible(&pitchShiftLabel_);
    pitchShiftLabel_.setFont(Font (12.0f));

    sweepWidthLabel_.attachToComponent(&sweepWidthSlider_, false);
    sweepWidthLabel_.setFont(Font (11.0f));
    
    frequencyLabel_.attachToComponent(&frequencySlider_, false);
    frequencyLabel_.setFont(Font (11.0f));
    
    waveformLabel_.attachToComponent(&waveformComboBox_, false);
    waveformLabel_.setFont(Font (11.0f));
    
    interpolationLabel_.attachToComponent(&interpolationComboBox_, false);
    interpolationLabel_.setFont(Font (11.0f));
    
    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible(resizer_ = new ResizableCornerComponent (this, &resizeLimits_));
    resizeLimits_.setSizeLimits(370, 160, 600, 300);
    
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(ownerFilter->lastUIWidth_,
            ownerFilter->lastUIHeight_);
    
    startTimer(50);
}

VibratoAudioProcessorEditor::~VibratoAudioProcessorEditor()
{
}

//==============================================================================
void VibratoAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
}

void VibratoAudioProcessorEditor::resized()
{
    sweepWidthSlider_.setBounds (20, 20, 150, 40);
    frequencySlider_.setBounds(200, 20, 150, 40);
    pitchShiftLabel_.setBounds(20, 60, 350, 20);
    waveformComboBox_.setBounds(20, 100, 150, 30);
    interpolationComboBox_.setBounds(200, 100, 150, 30);
    
    resizer_->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
    
    getProcessor()->lastUIWidth_ = getWidth();
    getProcessor()->lastUIHeight_ = getHeight();
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void VibratoAudioProcessorEditor::timerCallback()
{
    VibratoAudioProcessor* ourProcessor = getProcessor();
    
    sweepWidthSlider_.setValue(ourProcessor->sweepWidth_, dontSendNotification);
    frequencySlider_.setValue(ourProcessor->frequency_, dontSendNotification);
    waveformComboBox_.setSelectedId(ourProcessor->waveform_, dontSendNotification);
    interpolationComboBox_.setSelectedId(ourProcessor->interpolation_, dontSendNotification);
    
    // Update the pitch shift label only when something changes to avoid
    // needless calculations
    if(ourProcessor->sweepWidth_ != oldSweepWidth_ ||
       ourProcessor->frequency_ != oldFrequency_ ||
       ourProcessor->waveform_ != oldWaveform_)
    {
        updatePitchShiftLabel();
        oldSweepWidth_ = ourProcessor->sweepWidth_;
        oldFrequency_ = ourProcessor->frequency_;
        oldWaveform_ = ourProcessor->waveform_;
    }
}

// This is our Slider::Listener callback, when the user drags a slider.
void VibratoAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
    // by the host, rather than just modifying them directly, otherwise the host won't know
    // that they've changed.
    
    if (slider == &sweepWidthSlider_)
    {
        getProcessor()->setParameterNotifyingHost (VibratoAudioProcessor::kSweepWidthParam,
                                                   (float)sweepWidthSlider_.getValue());
        updatePitchShiftLabel();
    }
    else if (slider == &frequencySlider_)
    {
        getProcessor()->setParameterNotifyingHost (VibratoAudioProcessor::kFrequencyParam,
                                                   (float)frequencySlider_.getValue());
        updatePitchShiftLabel();
    }
}

// Similar callback to sliderValueChanged for ComboBox updates
void VibratoAudioProcessorEditor::comboBoxChanged (ComboBox *comboBox)
{
    if(comboBox == &waveformComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (VibratoAudioProcessor::kWaveformParam,
                                                   (float)waveformComboBox_.getSelectedId());
        updatePitchShiftLabel();
    }
    else if(comboBox == &interpolationComboBox_)
    {
        getProcessor()->setParameterNotifyingHost (VibratoAudioProcessor::kInterpolationParam,
                                                   (float)interpolationComboBox_.getSelectedId());
        // This parameter doesn't affect the maximum pitch shift
    }
}

// Update the content of the pitch shift label (whenever controls change)
void VibratoAudioProcessorEditor::updatePitchShiftLabel()
{
    VibratoAudioProcessor* ourProcessor = getProcessor();
    
    // The amount of pitch shift depends on the derivative of the delay, which
    // is given by: delay = width * f(frequency * t)
    // where f(x) is one of:
    //   sine --> 0.5 + 0.5*sin(2*pi*x) --> derivative pi*cos(x)*dx
    //   triangle --> {2.0*x or 1.0-(2.0*(x-0.5)) ---> derivative +/- 2.0*dx
    //   sawtooth rising --> x --> derivative 1.0*dx
    //   sawtooth falling --> 1.0 - x --> derivative -1.0*dx
    // For f(frequency*t), "dx" = frequency
    
    float maxSpeed = 1.0, minSpeed = 1.0;
    float maxPitch = 0.0, minPitch = 0.0;
    char str[256];
    
    switch(ourProcessor->waveform_)
    {
        case VibratoAudioProcessor::kWaveformSine:
            maxSpeed = 1.0 + M_PI * ourProcessor->frequency_ * ourProcessor->sweepWidth_;
            minSpeed = 1.0 - M_PI * ourProcessor->frequency_ * ourProcessor->sweepWidth_;
            break;
        case VibratoAudioProcessor::kWaveformTriangle:
            maxSpeed = 1.0 + 2.0 * ourProcessor->frequency_ * ourProcessor->sweepWidth_;
            minSpeed = 1.0 - 2.0 * ourProcessor->frequency_ * ourProcessor->sweepWidth_;
            break;
        case VibratoAudioProcessor::kWaveformSawtooth:
            // Standard (rising) sawtooth means delay is increasing --> pitch is lower
            maxSpeed = 1.0;
            minSpeed = 1.0 - ourProcessor->frequency_ * ourProcessor->sweepWidth_;
            break;
        case VibratoAudioProcessor::kWaveformInverseSawtooth:
            // Inverse (falling) sawtooth means delay is decreasing --> pitch is higher
            maxSpeed = 1.0 + ourProcessor->frequency_ * ourProcessor->sweepWidth_;
            minSpeed = 1.0;
            break;
    }
    
    // Convert speed to pitch shift --> semitones = 12*log2(speed)
    maxPitch = 12.0*logf(maxSpeed)/logf(2.0);
    
    if(minSpeed > 0)
    {
        minPitch = 12.0*logf(minSpeed)/logf(2.0);
        snprintf(str, 256, "Vibrato range: %+.2f to %+.2f semitones (speed %.3f to %.3f)",
                 minPitch, maxPitch, minSpeed, maxSpeed);
    }
    else
    {
        snprintf(str, 256, "Vibrato range: --- to %+.2f semitones (speed %.3f to %.3f)",
                 maxPitch, minSpeed, maxSpeed);
    }
    
    pitchShiftLabel_.setText(str, dontSendNotification);
}