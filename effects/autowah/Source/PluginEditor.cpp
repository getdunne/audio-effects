/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "PluginProcessor.h"
//[/Headers]

#include "PluginEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

//LookAndFeelCustom* AutoWahAudioProcessorEditor::customLookAndFeel = NULL;

//[/MiscUserDefs]

//==============================================================================
AutoWahAudioProcessorEditor::AutoWahAudioProcessorEditor (AutoWahAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter)
{
    addAndMakeVisible (baseFrequencySlider_ = new Slider ("new slider"));
    baseFrequencySlider_->setRange (200, 1000, 0.1);
    baseFrequencySlider_->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    baseFrequencySlider_->setTextBoxStyle (Slider::TextBoxBelow, false, 60, 20);
    baseFrequencySlider_->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    baseFrequencySlider_->setColour (Slider::textBoxTextColourId, Colours::white);
    baseFrequencySlider_->setColour (Slider::textBoxBackgroundColourId, Colour (0x00000000));
    baseFrequencySlider_->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    baseFrequencySlider_->addListener (this);

    addAndMakeVisible (qSlider_ = new Slider ("new slider"));
    qSlider_->setRange (2, 20, 0.1);
    qSlider_->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    qSlider_->setTextBoxStyle (Slider::TextBoxBelow, false, 60, 20);
    qSlider_->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    qSlider_->setColour (Slider::textBoxTextColourId, Colours::white);
    qSlider_->setColour (Slider::textBoxBackgroundColourId, Colour (0x00000000));
    qSlider_->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    qSlider_->addListener (this);

    addAndMakeVisible (lfoFrequencySlider_ = new Slider ("new slider"));
    lfoFrequencySlider_->setRange (0.2, 20, 0.1);
    lfoFrequencySlider_->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    lfoFrequencySlider_->setTextBoxStyle (Slider::TextBoxBelow, false, 60, 20);
    lfoFrequencySlider_->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    lfoFrequencySlider_->setColour (Slider::textBoxTextColourId, Colours::white);
    lfoFrequencySlider_->setColour (Slider::textBoxBackgroundColourId, Colour (0x00000000));
    lfoFrequencySlider_->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    lfoFrequencySlider_->addListener (this);

    addAndMakeVisible (lfoWidthSlider_ = new Slider ("new slider"));
    lfoWidthSlider_->setRange (0, 2000, 0.1);
    lfoWidthSlider_->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    lfoWidthSlider_->setTextBoxStyle (Slider::TextBoxBelow, false, 60, 20);
    lfoWidthSlider_->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    lfoWidthSlider_->setColour (Slider::textBoxTextColourId, Colours::white);
    lfoWidthSlider_->setColour (Slider::textBoxBackgroundColourId, Colour (0x00000000));
    lfoWidthSlider_->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    lfoWidthSlider_->addListener (this);

    addAndMakeVisible (envelopeWidthSlider_ = new Slider ("new slider"));
    envelopeWidthSlider_->setRange (0, 4000, 0.1);
    envelopeWidthSlider_->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    envelopeWidthSlider_->setTextBoxStyle (Slider::TextBoxBelow, false, 60, 20);
    envelopeWidthSlider_->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    envelopeWidthSlider_->setColour (Slider::textBoxTextColourId, Colours::white);
    envelopeWidthSlider_->setColour (Slider::textBoxBackgroundColourId, Colour (0x00000000));
    envelopeWidthSlider_->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    envelopeWidthSlider_->addListener (this);

    addAndMakeVisible (envelopeAttackSlider_ = new Slider ("new slider"));
    envelopeAttackSlider_->setRange (0, 0.1, 0.0001);
    envelopeAttackSlider_->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    envelopeAttackSlider_->setTextBoxStyle (Slider::TextBoxBelow, false, 60, 20);
    envelopeAttackSlider_->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    envelopeAttackSlider_->setColour (Slider::textBoxTextColourId, Colours::white);
    envelopeAttackSlider_->setColour (Slider::textBoxBackgroundColourId, Colour (0x00000000));
    envelopeAttackSlider_->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    envelopeAttackSlider_->addListener (this);
    envelopeAttackSlider_->setSkewFactor (0.02);

    addAndMakeVisible (envelopeDecaySlider_ = new Slider ("new slider"));
    envelopeDecaySlider_->setRange (0, 2, 0.001);
    envelopeDecaySlider_->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    envelopeDecaySlider_->setTextBoxStyle (Slider::TextBoxBelow, false, 60, 20);
    envelopeDecaySlider_->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    envelopeDecaySlider_->setColour (Slider::textBoxTextColourId, Colours::white);
    envelopeDecaySlider_->setColour (Slider::textBoxBackgroundColourId, Colour (0x00000000));
    envelopeDecaySlider_->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    envelopeDecaySlider_->addListener (this);
    envelopeDecaySlider_->setSkewFactor (0.4);


    //[UserPreSize]
    addAndMakeVisible(resizer_ = new ResizableCornerComponent (this, &resizeLimits_));
    resizeLimits_.setSizeLimits(550, 200, 550, 200);
    //[/UserPreSize]

    setSize (354, 380);


    //[Constructor] You can add your own custom stuff here..
	//AutoWahAudioProcessor* ourProcessor = getProcessor();   // UNUSED (include when necessary)

    UpdateInterfaceValues();

    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(ownerFilter->lastUIWidth_,
            ownerFilter->lastUIHeight_);

    //************************************ SET LOOKANDFEEL **********************************
    //	if( customLookAndFeel == NULL )
    //	{
    //		customLookAndFeel = new LookAndFeelCustom();
    //		LookAndFeel::setDefaultLookAndFeel (/*&*/customLookAndFeel);
    //	}

	//***************************************************************************************


    startTimer(50);

    //[/Constructor]
}

AutoWahAudioProcessorEditor::~AutoWahAudioProcessorEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    baseFrequencySlider_ = nullptr;
    qSlider_ = nullptr;
    lfoFrequencySlider_ = nullptr;
    lfoWidthSlider_ = nullptr;
    envelopeWidthSlider_ = nullptr;
    envelopeAttackSlider_ = nullptr;
    envelopeDecaySlider_ = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AutoWahAudioProcessorEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffa7998c));

    g.setColour (Colour (0xff9e8365));
    g.fillRoundedRectangle (11.0f, 12.0f, 333.0f, 94.0f, 10.000f);

    g.setColour (Colour (0xff9e8365));
    g.fillRoundedRectangle (11.0f, 115.0f, 333.0f, 94.0f, 10.000f);

    g.setColour (Colour (0xff9e8365));
    g.fillRoundedRectangle (11.0f, 217.0f, 333.0f, 152.0f, 10.000f);

    g.setColour (Colour (0xff992b13));
    g.setFont (Font (15.00f, Font::plain));
    g.drawText (TRANS("Q"),
                190, 32, 72, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff992b13));
    g.setFont (Font (15.00f, Font::plain));
    g.drawText (TRANS("Depth"),
                190, 138, 72, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff992b13));
    g.setFont (Font (15.00f, Font::plain));
    g.drawText (TRANS("Attack"),
                190, 233, 72, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff992b13));
    g.setFont (Font (15.00f, Font::plain));
    g.drawText (TRANS("Release"),
                192, 304, 72, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff992b13));
    g.setFont (Font (15.00f, Font::plain));
    g.drawText (TRANS("Frequency"),
                12, 32, 72, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff992b13));
    g.setFont (Font (15.00f, Font::plain));
    g.drawText (TRANS("Frequency"),
                3, 138, 89, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff992b13));
    g.setFont (Font (15.00f, Font::plain));
    g.drawText (TRANS("Depth"),
                0, 233, 95, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff992b13));
    g.setFont (Font (28.00f, Font::bold));
    g.drawText (TRANS("AutoWah"),
                2, -2, 111, 28,
                Justification::centred, true);

    g.setColour (Colour (0xff992b13));
    g.setFont (Font (24.00f, Font::plain));
    g.drawText (TRANS("LFO"),
                1, 176, 69, 34,
                Justification::centred, true);

    g.setColour (Colour (0xff992b13));
    g.setFont (Font (24.00f, Font::plain));
    g.drawText (TRANS("ENVELOPE"),
                10, 334, 112, 34,
                Justification::centred, true);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AutoWahAudioProcessorEditor::resized()
{
    baseFrequencySlider_->setBounds (86, 20, 80, 80);
    qSlider_->setBounds (261, 20, 80, 80);
    lfoFrequencySlider_->setBounds (86, 126, 80, 80);
    lfoWidthSlider_->setBounds (261, 126, 80, 80);
    envelopeWidthSlider_->setBounds (86, 223, 80, 80);
    envelopeAttackSlider_->setBounds (269, 223, 66, 66);
    envelopeDecaySlider_->setBounds (270, 299, 66, 66);
    //[UserResized] Add your own custom resize handling here..
    getProcessor()->lastUIWidth_ = getWidth();
    getProcessor()->lastUIHeight_ = getHeight();
    //[/UserResized]
}

void AutoWahAudioProcessorEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    AutoWahAudioProcessor* ourProcessor = getProcessor();
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == baseFrequencySlider_)
    {
        //[UserSliderCode_baseFrequencySlider_] -- add your slider handling code here..
        ourProcessor->setParameterNotifyingHost (AutoWahAudioProcessor::kBaseFrequencyParam,
                                                   (float)baseFrequencySlider_->getValue());
        //[/UserSliderCode_baseFrequencySlider_]
    }
    else if (sliderThatWasMoved == qSlider_)
    {
        //[UserSliderCode_qSlider_] -- add your slider handling code here..
        ourProcessor->setParameterNotifyingHost (AutoWahAudioProcessor::kQParam,
                                                   (float)qSlider_->getValue());
        //[/UserSliderCode_qSlider_]
    }
    else if (sliderThatWasMoved == lfoFrequencySlider_)
    {
        //[UserSliderCode_lfoFrequencySlider_] -- add your slider handling code here..
        ourProcessor->setParameterNotifyingHost (AutoWahAudioProcessor::kLFOFrequencyParam,
                                                   (float)lfoFrequencySlider_->getValue());
        //[/UserSliderCode_lfoFrequencySlider_]
    }
    else if (sliderThatWasMoved == lfoWidthSlider_)
    {
        //[UserSliderCode_lfoWidthSlider_] -- add your slider handling code here..
        ourProcessor->setParameterNotifyingHost (AutoWahAudioProcessor::kLFOWidthParam,
                                                   (float)lfoWidthSlider_->getValue());
        //[/UserSliderCode_lfoWidthSlider_]
    }
    else if (sliderThatWasMoved == envelopeWidthSlider_)
    {
        //[UserSliderCode_envelopeWidthSlider_] -- add your slider handling code here..
        ourProcessor->setParameterNotifyingHost (AutoWahAudioProcessor::kEnvelopeWidthParam,
                                                   (float)envelopeWidthSlider_->getValue());
        //[/UserSliderCode_envelopeWidthSlider_]
    }
    else if (sliderThatWasMoved == envelopeAttackSlider_)
    {
        //[UserSliderCode_envelopeAttackSlider_] -- add your slider handling code here..
        ourProcessor->setParameterNotifyingHost (AutoWahAudioProcessor::kEnvelopeAttackParam,
                                                   (float)envelopeAttackSlider_->getValue());
        //[/UserSliderCode_envelopeAttackSlider_]
    }
    else if (sliderThatWasMoved == envelopeDecaySlider_)
    {
        //[UserSliderCode_envelopeDecaySlider_] -- add your slider handling code here..
        ourProcessor->setParameterNotifyingHost (AutoWahAudioProcessor::kEnvelopeDecayParam,
                                                   (float)envelopeDecaySlider_->getValue());
        //[/UserSliderCode_envelopeDecaySlider_]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void AutoWahAudioProcessorEditor::UpdateInterfaceValues()
{
	AutoWahAudioProcessor* ourProcessor = getProcessor();

    // Update Sliders
    baseFrequencySlider_    ->setValue(ourProcessor->baseFrequency_, dontSendNotification);
    qSlider_                ->setValue(ourProcessor->q_, dontSendNotification);
    lfoFrequencySlider_     ->setValue(ourProcessor->lfoFrequency_);
    lfoWidthSlider_         ->setValue(ourProcessor->lfoWidth_);
    envelopeWidthSlider_    ->setValue(ourProcessor->envelopeWidth_);
    envelopeAttackSlider_   ->setValue(ourProcessor->envelopeAttack_);
    envelopeDecaySlider_    ->setValue(ourProcessor->envelopeDecay_);

}

// This timer periodically checks whether any of the filter's parameters have changed...
void AutoWahAudioProcessorEditor::timerCallback()
{
    // AutoWahAudioProcessor* ourProcessor = getProcessor();
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AutoWahAudioProcessorEditor"
                 componentName="" parentClasses="public AudioProcessorEditor, public Timer"
                 constructorParams="AutoWahAudioProcessor* ownerFilter" variableInitialisers="AudioProcessorEditor (ownerFilter)"
                 snapPixels="8" snapActive="0" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="354" initialHeight="380">
  <BACKGROUND backgroundColour="ffa7998c">
    <ROUNDRECT pos="11 12 333 94" cornerSize="10" fill="solid: ff9e8365" hasStroke="0"/>
    <ROUNDRECT pos="11 115 333 94" cornerSize="10" fill="solid: ff9e8365" hasStroke="0"/>
    <ROUNDRECT pos="11 217 333 152" cornerSize="10" fill="solid: ff9e8365" hasStroke="0"/>
    <TEXT pos="190 32 72 30" fill="solid: ff992b13" hasStroke="0" text="Q"
          fontname="Default font" fontsize="15" bold="0" italic="0" justification="36"/>
    <TEXT pos="190 138 72 30" fill="solid: ff992b13" hasStroke="0" text="Depth"
          fontname="Default font" fontsize="15" bold="0" italic="0" justification="36"/>
    <TEXT pos="190 233 72 30" fill="solid: ff992b13" hasStroke="0" text="Attack"
          fontname="Default font" fontsize="15" bold="0" italic="0" justification="36"/>
    <TEXT pos="192 304 72 30" fill="solid: ff992b13" hasStroke="0" text="Release"
          fontname="Default font" fontsize="15" bold="0" italic="0" justification="36"/>
    <TEXT pos="12 32 72 30" fill="solid: ff992b13" hasStroke="0" text="Frequency"
          fontname="Default font" fontsize="15" bold="0" italic="0" justification="36"/>
    <TEXT pos="3 138 89 30" fill="solid: ff992b13" hasStroke="0" text="Frequency"
          fontname="Default font" fontsize="15" bold="0" italic="0" justification="36"/>
    <TEXT pos="0 233 95 30" fill="solid: ff992b13" hasStroke="0" text="Depth"
          fontname="Default font" fontsize="15" bold="0" italic="0" justification="36"/>
    <TEXT pos="2 -2 111 28" fill="solid: ff992b13" hasStroke="0" text="AutoWah"
          fontname="Default font" fontsize="28" bold="1" italic="0" justification="36"/>
    <TEXT pos="1 176 69 34" fill="solid: ff992b13" hasStroke="0" text="LFO"
          fontname="Default font" fontsize="24" bold="0" italic="0" justification="36"/>
    <TEXT pos="10 334 112 34" fill="solid: ff992b13" hasStroke="0" text="ENVELOPE"
          fontname="Default font" fontsize="24" bold="0" italic="0" justification="36"/>
  </BACKGROUND>
  <SLIDER name="new slider" id="b7cd5ae6d0a3bfc6" memberName="baseFrequencySlider_"
          virtualName="" explicitFocusOrder="0" pos="86 20 80 80" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="0" textboxoutline="808080"
          min="200" max="1000" int="0.10000000000000000555" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="60"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="d914eb36766034b1" memberName="qSlider_"
          virtualName="" explicitFocusOrder="0" pos="261 20 80 80" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="0" textboxoutline="808080"
          min="2" max="20" int="0.10000000000000000555" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="60"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="d914eb36766034b2" memberName="lfoFrequencySlider_"
          virtualName="" explicitFocusOrder="0" pos="86 126 80 80" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="0" textboxoutline="808080"
          min="0.2000000000000000111" max="20" int="0.10000000000000000555"
          style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="60" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="d914eb36766034b3" memberName="lfoWidthSlider_"
          virtualName="" explicitFocusOrder="0" pos="261 126 80 80" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="0" textboxoutline="808080"
          min="0" max="2000" int="0.10000000000000000555" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="60"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="d914eb36766034b4" memberName="envelopeWidthSlider_"
          virtualName="" explicitFocusOrder="0" pos="86 223 80 80" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="0" textboxoutline="808080"
          min="0" max="4000" int="0.10000000000000000555" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="60"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="d914eb36766034b5" memberName="envelopeAttackSlider_"
          virtualName="" explicitFocusOrder="0" pos="269 223 66 66" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="0" textboxoutline="808080"
          min="0" max="0.10000000000000000555" int="0.00010000000000000000479"
          style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="60" textBoxHeight="20" skewFactor="0.020000000000000000416"/>
  <SLIDER name="new slider" id="d914eb36766034b6" memberName="envelopeDecaySlider_"
          virtualName="" explicitFocusOrder="0" pos="270 299 66 66" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="0" textboxoutline="808080"
          min="0" max="2" int="0.0010000000000000000208" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="60"
          textBoxHeight="20" skewFactor="0.4000000000000000222"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
