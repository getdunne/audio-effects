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
#include "MVerb.h"
//[/Headers]

#include "PluginEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

//LookAndFeelCustom* ReverbAudioProcessorEditor::customLookAndFeel = NULL;

//[/MiscUserDefs]

//==============================================================================
ReverbAudioProcessorEditor::ReverbAudioProcessorEditor (ReverbAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter)
{
    addAndMakeVisible (lateEarlySlider = new Slider ("new slider"));
    lateEarlySlider->setRange (0, 1, 0.01);
    lateEarlySlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    lateEarlySlider->setTextBoxStyle (Slider::TextBoxBelow, false, 40, 20);
    lateEarlySlider->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    lateEarlySlider->setColour (Slider::textBoxTextColourId, Colours::white);
    lateEarlySlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    lateEarlySlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    lateEarlySlider->addListener (this);

    addAndMakeVisible (mixSlider = new Slider ("new slider"));
    mixSlider->setRange (0, 1, 0.01);
    mixSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    mixSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 40, 20);
    mixSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    mixSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    mixSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    mixSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    mixSlider->addListener (this);

    addAndMakeVisible (gainSlider = new Slider ("new slider"));
    gainSlider->setRange (0, 1, 0.01);
    gainSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    gainSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 40, 20);
    gainSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    gainSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    gainSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    gainSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    gainSlider->addListener (this);

    addAndMakeVisible (preDelaySlider = new Slider ("new slider"));
    preDelaySlider->setRange (0, 1, 0.01);
    preDelaySlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    preDelaySlider->setTextBoxStyle (Slider::TextBoxBelow, false, 40, 20);
    preDelaySlider->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    preDelaySlider->setColour (Slider::textBoxTextColourId, Colours::white);
    preDelaySlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    preDelaySlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    preDelaySlider->addListener (this);

    addAndMakeVisible (bandWidthSlider = new Slider ("new slider"));
    bandWidthSlider->setRange (0, 1, 0.01);
    bandWidthSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    bandWidthSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 40, 20);
    bandWidthSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    bandWidthSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    bandWidthSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    bandWidthSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    bandWidthSlider->addListener (this);

    addAndMakeVisible (dampSlider = new Slider ("new slider"));
    dampSlider->setRange (0, 1, 0.01);
    dampSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    dampSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 40, 20);
    dampSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    dampSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    dampSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    dampSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    dampSlider->addListener (this);

    addAndMakeVisible (sizeSlider = new Slider ("new slider"));
    sizeSlider->setRange (0, 1, 0.01);
    sizeSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    sizeSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 40, 20);
    sizeSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    sizeSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    sizeSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    sizeSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    sizeSlider->addListener (this);

    addAndMakeVisible (decaySlider = new Slider ("new slider"));
    decaySlider->setRange (0, 1, 0.01);
    decaySlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    decaySlider->setTextBoxStyle (Slider::TextBoxBelow, false, 40, 20);
    decaySlider->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    decaySlider->setColour (Slider::textBoxTextColourId, Colours::white);
    decaySlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    decaySlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    decaySlider->addListener (this);

    addAndMakeVisible (densitySlider = new Slider ("new slider"));
    densitySlider->setRange (0, 1, 0.01);
    densitySlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    densitySlider->setTextBoxStyle (Slider::TextBoxBelow, false, 40, 20);
    densitySlider->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    densitySlider->setColour (Slider::textBoxTextColourId, Colours::white);
    densitySlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    densitySlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    densitySlider->addListener (this);


    //[UserPreSize]
    addAndMakeVisible (resizer = new ResizableCornerComponent (this, &resizeLimits));
    resizeLimits.setSizeLimits (150, 150, 850, 290);
    //[/UserPreSize]

    setSize (470, 470);


    //[Constructor] You can add your own custom stuff here..

	UpdateInterfaceValues();

	//************************************ SET LOOKANDFEEL **********************************
//	if( customLookAndFeel == NULL )
//	{
//		customLookAndFeel = new LookAndFeelCustom();
//		LookAndFeel::setDefaultLookAndFeel (/*&*/customLookAndFeel);
//	}

	//***************************************************************************************

    startTimer (50);

    //[/Constructor]
}

ReverbAudioProcessorEditor::~ReverbAudioProcessorEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    lateEarlySlider = nullptr;
    mixSlider = nullptr;
    gainSlider = nullptr;
    preDelaySlider = nullptr;
    bandWidthSlider = nullptr;
    dampSlider = nullptr;
    sizeSlider = nullptr;
    decaySlider = nullptr;
    densitySlider = nullptr;


    //[Destructor]. You can add your own custom destruction code here..

    //[/Destructor]
}

//==============================================================================
void ReverbAudioProcessorEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff000a39));

    g.setColour (Colour (0xff315d9d));
    g.fillEllipse (332.0f, 204.0f, 88.0f, 88.0f);

    g.setColour (Colour (0xff2c333b));
    g.fillEllipse (348.0f, 220.0f, 56.0f, 56.0f);

    g.setColour (Colour (0xff2f3d6a));
    g.fillRoundedRectangle (136.0f, 5.0f, 175.0f, 37.0f, 10.000f);

    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 22.30f, Font::bold));
    g.drawText (TRANS("reverb"),
                1, 13, 446, 24,
                Justification::centredTop, true);

    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 15.00f, Font::plain));
    g.drawText (TRANS("MIX"),
                346, 183, 59, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff315d9d));
    g.fillEllipse (332.0f, 337.0f, 88.0f, 88.0f);

    g.setColour (Colour (0xff2c333b));
    g.fillEllipse (348.0f, 353.0f, 56.0f, 56.0f);

    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 15.00f, Font::plain));
    g.drawText (TRANS("LATE/EARLY"),
                333, 316, 83, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff315d9d));
    g.fillEllipse (332.0f, 78.0f, 88.0f, 88.0f);

    g.setColour (Colour (0xff2c333b));
    g.fillEllipse (348.0f, 94.0f, 56.0f, 56.0f);

    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 15.00f, Font::plain));
    g.drawText (TRANS("GAIN"),
                346, 57, 59, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff315d9d));
    g.fillEllipse (190.0f, 204.0f, 88.0f, 88.0f);

    g.setColour (Colour (0xff2c333b));
    g.fillEllipse (206.0f, 220.0f, 56.0f, 56.0f);

    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 15.00f, Font::plain));
    g.drawText (TRANS("BANDWITH"),
                201, 183, 65, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff315d9d));
    g.fillEllipse (190.0f, 337.0f, 88.0f, 88.0f);

    g.setColour (Colour (0xff2c333b));
    g.fillEllipse (206.0f, 353.0f, 56.0f, 56.0f);

    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 15.00f, Font::plain));
    g.drawText (TRANS("PREDELAY"),
                202, 316, 64, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff315d9d));
    g.fillEllipse (190.0f, 78.0f, 88.0f, 88.0f);

    g.setColour (Colour (0xff2c333b));
    g.fillEllipse (206.0f, 94.0f, 56.0f, 56.0f);

    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 15.00f, Font::plain));
    g.drawText (TRANS("DAMP"),
                204, 57, 59, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff315d9d));
    g.fillEllipse (48.0f, 204.0f, 88.0f, 88.0f);

    g.setColour (Colour (0xff2c333b));
    g.fillEllipse (64.0f, 220.0f, 56.0f, 56.0f);

    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 15.00f, Font::plain));
    g.drawText (TRANS("DECAY"),
                62, 183, 59, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff315d9d));
    g.fillEllipse (48.0f, 337.0f, 88.0f, 88.0f);

    g.setColour (Colour (0xff2c333b));
    g.fillEllipse (64.0f, 353.0f, 56.0f, 56.0f);

    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 15.00f, Font::plain));
    g.drawText (TRANS("SIZE"),
                62, 316, 59, 30,
                Justification::centred, true);

    g.setColour (Colour (0xff315d9d));
    g.fillEllipse (48.0f, 78.0f, 88.0f, 88.0f);

    g.setColour (Colour (0xff2c333b));
    g.fillEllipse (64.0f, 94.0f, 56.0f, 56.0f);

    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 15.00f, Font::plain));
    g.drawText (TRANS("DENSITY"),
                63, 57, 59, 30,
                Justification::centred, true);

    //[UserPaint] Add your own custom painting code here..

    // Version number
    String str("v ");
    str.append(JucePlugin_VersionString, 4);
    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 11.00f, Font::plain));
    g.drawText (str,
                189, 37, 54, 30,
                Justification::centredLeft, true);

    //[/UserPaint]
}

void ReverbAudioProcessorEditor::resized()
{
    lateEarlySlider->setBounds (294, 339, 164, 102);
    mixSlider->setBounds (294, 207, 164, 102);
    gainSlider->setBounds (294, 81, 164, 102);
    preDelaySlider->setBounds (152, 340, 164, 102);
    bandWidthSlider->setBounds (152, 207, 164, 102);
    dampSlider->setBounds (152, 80, 164, 102);
    sizeSlider->setBounds (11, 340, 164, 102);
    decaySlider->setBounds (10, 207, 164, 102);
    densitySlider->setBounds (10, 81, 164, 102);
    //[UserResized] Add your own custom resize handling here..
    getProcessor()->_lastUIWidth = getWidth();
    getProcessor()->_lastUIHeight = getHeight();
    //[/UserResized]
}

void ReverbAudioProcessorEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    ReverbAudioProcessor* ourProcessor = getProcessor();
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == lateEarlySlider)
    {
        //[UserSliderCode_lateEarlySlider] -- add your slider handling code here..
        ourProcessor->SetLateEarly(lateEarlySlider->getValue());
        ourProcessor->updateParameters(MVerb<float>::EARLYMIX);
        //[/UserSliderCode_lateEarlySlider]
    }
    else if (sliderThatWasMoved == mixSlider)
    {
        //[UserSliderCode_mixSlider] -- add your slider handling code here..
        ourProcessor->SetMix(mixSlider->getValue());
        ourProcessor->updateParameters(MVerb<float>::MIX);
        //[/UserSliderCode_mixSlider]
    }
    else if (sliderThatWasMoved == gainSlider)
    {
        //[UserSliderCode_gainSlider] -- add your slider handling code here..
        ourProcessor->SetGain(gainSlider->getValue());
        ourProcessor->updateParameters(MVerb<float>::GAIN);
        //[/UserSliderCode_gainSlider]
    }
    else if (sliderThatWasMoved == preDelaySlider)
    {
        //[UserSliderCode_preDelaySlider] -- add your slider handling code here..
        ourProcessor->SetPredelay(preDelaySlider->getValue());
        ourProcessor->updateParameters(MVerb<float>::PREDELAY);
        //[/UserSliderCode_preDelaySlider]
    }
    else if (sliderThatWasMoved == bandWidthSlider)
    {
        //[UserSliderCode_bandWidthSlider] -- add your slider handling code here..
        ourProcessor->SetBandwidth(bandWidthSlider->getValue());
        ourProcessor->updateParameters(MVerb<float>::BANDWIDTHFREQ);
        //[/UserSliderCode_bandWidthSlider]
    }
    else if (sliderThatWasMoved == dampSlider)
    {
        //[UserSliderCode_dampSlider] -- add your slider handling code here..
        ourProcessor->SetDamp(dampSlider->getValue());
        ourProcessor->updateParameters(MVerb<float>::DAMPINGFREQ);
        //[/UserSliderCode_dampSlider]
    }
    else if (sliderThatWasMoved == sizeSlider)
    {
        //[UserSliderCode_sizeSlider] -- add your slider handling code here..
        ourProcessor->SetSize(sizeSlider->getValue());
        ourProcessor->updateParameters(MVerb<float>::SIZE);
        //[/UserSliderCode_sizeSlider]
    }
    else if (sliderThatWasMoved == decaySlider)
    {
        //[UserSliderCode_decaySlider] -- add your slider handling code here..
        ourProcessor->SetDecay(decaySlider->getValue());
        ourProcessor->updateParameters(MVerb<float>::DECAY);
        //[/UserSliderCode_decaySlider]
    }
    else if (sliderThatWasMoved == densitySlider)
    {
        //[UserSliderCode_densitySlider] -- add your slider handling code here..
        ourProcessor->SetDensity(densitySlider->getValue());
        ourProcessor->updateParameters(MVerb<float>::DENSITY);
        //[/UserSliderCode_densitySlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void ReverbAudioProcessorEditor::UpdateInterfaceValues()
{
	ReverbAudioProcessor* ourProcessor = getProcessor();

    // Update Sliders
    lateEarlySlider	->setValue(ourProcessor->_lateEarly,dontSendNotification);
    densitySlider	->setValue(ourProcessor->_density,dontSendNotification);
    mixSlider       ->setValue(ourProcessor->_mix,dontSendNotification);
    preDelaySlider	->setValue(ourProcessor->_predelay,dontSendNotification);
    dampSlider      ->setValue(ourProcessor->_damp,dontSendNotification);
    sizeSlider      ->setValue(ourProcessor->_size,dontSendNotification);
    decaySlider     ->setValue(ourProcessor->_decay,dontSendNotification);
    bandWidthSlider	->setValue(ourProcessor->_bandwidth,dontSendNotification);
    gainSlider      ->setValue(ourProcessor->_gain,dontSendNotification);
}

void ReverbAudioProcessorEditor::timerCallback()
{
	//DistortionAudioProcessor* ourProcessor = getProcessor();


}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ReverbAudioProcessorEditor"
                 componentName="" parentClasses="public AudioProcessorEditor, public Timer"
                 constructorParams="ReverbAudioProcessor* ownerFilter" variableInitialisers="AudioProcessorEditor (ownerFilter)"
                 snapPixels="8" snapActive="0" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="470" initialHeight="470">
  <BACKGROUND backgroundColour="ff000a39">
    <ELLIPSE pos="332 204 88 88" fill="solid: ff315d9d" hasStroke="0"/>
    <ELLIPSE pos="348 220 56 56" fill="solid: ff2c333b" hasStroke="0"/>
    <ROUNDRECT pos="136 5 175 37" cornerSize="10" fill="solid: ff2f3d6a" hasStroke="0"/>
    <TEXT pos="1 13 446 24" fill="solid: ffffffff" hasStroke="0" text="reverb"
          fontname="Default monospaced font" fontsize="22.300000000000000711"
          bold="1" italic="0" justification="12"/>
    <TEXT pos="346 183 59 30" fill="solid: ffffffff" hasStroke="0" text="MIX"
          fontname="Default monospaced font" fontsize="15" bold="0" italic="0"
          justification="36"/>
    <ELLIPSE pos="332 337 88 88" fill="solid: ff315d9d" hasStroke="0"/>
    <ELLIPSE pos="348 353 56 56" fill="solid: ff2c333b" hasStroke="0"/>
    <TEXT pos="333 316 83 30" fill="solid: ffffffff" hasStroke="0" text="LATE/EARLY"
          fontname="Default monospaced font" fontsize="15" bold="0" italic="0"
          justification="36"/>
    <ELLIPSE pos="332 78 88 88" fill="solid: ff315d9d" hasStroke="0"/>
    <ELLIPSE pos="348 94 56 56" fill="solid: ff2c333b" hasStroke="0"/>
    <TEXT pos="346 57 59 30" fill="solid: ffffffff" hasStroke="0" text="GAIN"
          fontname="Default monospaced font" fontsize="15" bold="0" italic="0"
          justification="36"/>
    <ELLIPSE pos="190 204 88 88" fill="solid: ff315d9d" hasStroke="0"/>
    <ELLIPSE pos="206 220 56 56" fill="solid: ff2c333b" hasStroke="0"/>
    <TEXT pos="201 183 65 30" fill="solid: ffffffff" hasStroke="0" text="BANDWITH"
          fontname="Default monospaced font" fontsize="15" bold="0" italic="0"
          justification="36"/>
    <ELLIPSE pos="190 337 88 88" fill="solid: ff315d9d" hasStroke="0"/>
    <ELLIPSE pos="206 353 56 56" fill="solid: ff2c333b" hasStroke="0"/>
    <TEXT pos="202 316 64 30" fill="solid: ffffffff" hasStroke="0" text="PREDELAY"
          fontname="Default monospaced font" fontsize="15" bold="0" italic="0"
          justification="36"/>
    <ELLIPSE pos="190 78 88 88" fill="solid: ff315d9d" hasStroke="0"/>
    <ELLIPSE pos="206 94 56 56" fill="solid: ff2c333b" hasStroke="0"/>
    <TEXT pos="204 57 59 30" fill="solid: ffffffff" hasStroke="0" text="DAMP"
          fontname="Default monospaced font" fontsize="15" bold="0" italic="0"
          justification="36"/>
    <ELLIPSE pos="48 204 88 88" fill="solid: ff315d9d" hasStroke="0"/>
    <ELLIPSE pos="64 220 56 56" fill="solid: ff2c333b" hasStroke="0"/>
    <TEXT pos="62 183 59 30" fill="solid: ffffffff" hasStroke="0" text="DECAY"
          fontname="Default monospaced font" fontsize="15" bold="0" italic="0"
          justification="36"/>
    <ELLIPSE pos="48 337 88 88" fill="solid: ff315d9d" hasStroke="0"/>
    <ELLIPSE pos="64 353 56 56" fill="solid: ff2c333b" hasStroke="0"/>
    <TEXT pos="62 316 59 30" fill="solid: ffffffff" hasStroke="0" text="SIZE"
          fontname="Default monospaced font" fontsize="15" bold="0" italic="0"
          justification="36"/>
    <ELLIPSE pos="48 78 88 88" fill="solid: ff315d9d" hasStroke="0"/>
    <ELLIPSE pos="64 94 56 56" fill="solid: ff2c333b" hasStroke="0"/>
    <TEXT pos="63 57 59 30" fill="solid: ffffffff" hasStroke="0" text="DENSITY"
          fontname="Default monospaced font" fontsize="15" bold="0" italic="0"
          justification="36"/>
  </BACKGROUND>
  <SLIDER name="new slider" id="b7cd5ae6d0a3bfc6" memberName="lateEarlySlider"
          virtualName="" explicitFocusOrder="0" pos="294 339 164 102" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="808080"
          min="0" max="1" int="0.010000000000000000208" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="40"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="8312c8e1102eca31" memberName="mixSlider"
          virtualName="" explicitFocusOrder="0" pos="294 207 164 102" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="808080"
          min="0" max="1" int="0.010000000000000000208" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="40"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="a0cf664fb4a85f44" memberName="gainSlider"
          virtualName="" explicitFocusOrder="0" pos="294 81 164 102" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="808080"
          min="0" max="1" int="0.010000000000000000208" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="40"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="55b568b536e67525" memberName="preDelaySlider"
          virtualName="" explicitFocusOrder="0" pos="152 340 164 102" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="808080"
          min="0" max="1" int="0.010000000000000000208" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="40"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="391314f65024cc7c" memberName="bandWidthSlider"
          virtualName="" explicitFocusOrder="0" pos="152 207 164 102" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="808080"
          min="0" max="1" int="0.010000000000000000208" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="40"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="8ec0d4dbfb348651" memberName="dampSlider"
          virtualName="" explicitFocusOrder="0" pos="152 80 164 102" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="808080"
          min="0" max="1" int="0.010000000000000000208" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="40"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="6120b55e7065ff03" memberName="sizeSlider"
          virtualName="" explicitFocusOrder="0" pos="11 340 164 102" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="808080"
          min="0" max="1" int="0.010000000000000000208" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="40"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="5aa10525b640e47e" memberName="decaySlider"
          virtualName="" explicitFocusOrder="0" pos="10 207 164 102" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="808080"
          min="0" max="1" int="0.010000000000000000208" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="40"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="new slider" id="4aebd74c8aeafbac" memberName="densitySlider"
          virtualName="" explicitFocusOrder="0" pos="10 81 164 102" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="808080"
          min="0" max="1" int="0.010000000000000000208" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="40"
          textBoxHeight="20" skewFactor="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
