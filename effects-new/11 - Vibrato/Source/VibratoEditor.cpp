
#include "VibratoProcessor.h"
#include "VibratoEditor.h"
#include "Vibrato.h"
#include "LFO_2.h"


VibratoEditor::VibratoEditor (VibratoProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , lfoFreqKnob(0.1f, 20.0f, "Hz")
    , sweepWidthKnob(0.0f, 1.0f, "%")
    , labeledLfoFreqKnob("LFO Freq", lfoFreqKnob)
    , labeledSweepWidthKnob("Width", sweepWidthKnob)

{
    
    
    setLookAndFeel(lookAndFeel);
    
    vibratoGroup.setText("Vibrato");
    addAndMakeVisible(&vibratoGroup);
    
    
    lfoWaveformLabel.setText("LFO", dontSendNotification);
    lfoWaveformLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&lfoWaveformLabel);
    
    lfoWaveformCombo.setEditableText(false);
    lfoWaveformCombo.setJustificationType(Justification::centredLeft);
    LFO_2::populateWaveformComboBox(lfoWaveformCombo);
    addAndMakeVisible(lfoWaveformCombo);
    
    interpolationLabel.setText("Type", dontSendNotification);
    interpolationLabel.setJustificationType(Justification::right);
    addAndMakeVisible(&interpolationLabel);
    
    interpolationCombo.setEditableText(false);
    interpolationCombo.setJustificationType(Justification::centredLeft);
    Vibrato::populateInterpolationComboBox(interpolationCombo);
    addAndMakeVisible(interpolationCombo);
    
    lfoFreqKnob.setDoubleClickReturnValue(true, 1.0f, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledLfoFreqKnob);
    
    sweepWidthKnob.setDoubleClickReturnValue(true, 0.5f, ModifierKeys::noModifiers);
    addAndMakeVisible(labeledSweepWidthKnob);
    
    processor.parameters.attachControls(lfoWaveformCombo, interpolationCombo, lfoFreqKnob, sweepWidthKnob);
    
    setSize (450, 325);
    
    
    
}

VibratoEditor::~VibratoEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}
//
//void VibratoProcessorEditor::updatePitchShiftLabel()
//{
//    VibratoProcessor* ourProcessor = getProcessor();
//
//    // The amount of pitch shift depends on the derivative of the delay, which
//    // is given by: delay = width * f(frequency * t)
//    // where f(x) is one of:
//    //   sine --> 0.5 + 0.5*sin(2*pi*x) --> derivative pi*cos(x)*dx
//    //   triangle --> {2.0*x or 1.0-(2.0*(x-0.5)) ---> derivative +/- 2.0*dx
//    //   sawtooth rising --> x --> derivative 1.0*dx
//    //   sawtooth falling --> 1.0 - x --> derivative -1.0*dx
//    // For f(frequency*t), "dx" = frequency
//
//    float maxSpeed = 1.0, minSpeed = 1.0;
//    float maxPitch = 0.0, minPitch = 0.0;
//    char str[256];
//
//    switch(ourProcessor->waveform_)
//    {
//        case VibratoProcessor::kWaveformSine:
//            maxSpeed = 1.0 + M_PI * ourProcessor->frequency_ * ourProcessor->sweepWidth_;
//            minSpeed = 1.0 - M_PI * ourProcessor->frequency_ * ourProcessor->sweepWidth_;
//            break;
//        case VibratoProcessor::kWaveformTriangle:
//            maxSpeed = 1.0 + 2.0 * ourProcessor->frequency_ * ourProcessor->sweepWidth_;
//            minSpeed = 1.0 - 2.0 * ourProcessor->frequency_ * ourProcessor->sweepWidth_;
//            break;
//        case VibratoProcessor::kWaveformSawtooth:
//            // Standard (rising) sawtooth means delay is increasing --> pitch is lower
//            maxSpeed = 1.0;
//            minSpeed = 1.0 - ourProcessor->frequency_ * ourProcessor->sweepWidth_;
//            break;
//        case VibratoProcessor::kWaveformInverseSawtooth:
//            // Inverse (falling) sawtooth means delay is decreasing --> pitch is higher
//            maxSpeed = 1.0 + ourProcessor->frequency_ * ourProcessor->sweepWidth_;
//            minSpeed = 1.0;
//            break;
//    }
//
//    // Convert speed to pitch shift --> semitones = 12*log2(speed)
//    maxPitch = 12.0*logf(maxSpeed)/logf(2.0);
//
//    if(minSpeed > 0)
//    {
//        minPitch = 12.0*logf(minSpeed)/logf(2.0);
//        snprintf(str, 256, "Vibrato range: %+.2f to %+.2f semitones (speed %.3f to %.3f)",
//                 minPitch, maxPitch, minSpeed, maxSpeed);
//    }
//    else
//    {
//        snprintf(str, 256, "Vibrato range: --- to %+.2f semitones (speed %.3f to %.3f)",
//                 maxPitch, minSpeed, maxSpeed);
//    }
//
////    pitchShiftLabel_.setText(str, dontSendNotification);
//}

//==============================================================================
void VibratoEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
 g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}

void VibratoEditor::resized()
{
    auto groupArea = getLocalBounds().reduced(20);
    vibratoGroup.setBounds(groupArea);
    
    auto widgetsArea = groupArea.reduced(20);
    widgetsArea.removeFromTop(12);
    auto waveformArea = widgetsArea.removeFromTop(24);
    lfoWaveformLabel.setBounds(waveformArea.removeFromLeft(40));
    lfoWaveformCombo.setBounds(waveformArea.removeFromLeft(getWidth()/2));
    auto interpolationArea = widgetsArea.removeFromTop(48);
    interpolationLabel.setBounds(interpolationArea.removeFromLeft(40));
    interpolationCombo.setBounds(interpolationArea.removeFromLeft(getWidth()/2));
    widgetsArea.removeFromTop(10);
    
    int knobWidth = widgetsArea.getWidth() / 2 - 10;
    labeledLfoFreqKnob.setBounds(widgetsArea.removeFromLeft(knobWidth));
    labeledSweepWidthKnob.setBounds(widgetsArea.removeFromRight(knobWidth));
}
