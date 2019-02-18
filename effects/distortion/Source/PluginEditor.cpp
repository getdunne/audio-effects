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

//LookAndFeelCustom* DistortionAudioProcessorEditor::customLookAndFeel = NULL;

//[/MiscUserDefs]

//==============================================================================
DistortionAudioProcessorEditor::DistortionAudioProcessorEditor (DistortionAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter)
{
    addAndMakeVisible (resetButton = new TextButton ("on off button"));
    resetButton->setButtonText (TRANS("RESET"));
    resetButton->addListener (this);
    resetButton->setColour (TextButton::buttonColourId, Colour (0xffad8585));
    resetButton->setColour (TextButton::buttonOnColourId, Colours::red);

    addAndMakeVisible (gainSlider = new Slider ("new slider"));
    gainSlider->setRange (-24, 24, 0.01);
    gainSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    gainSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 40, 20);
    gainSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x7fffffff));
    gainSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    gainSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    gainSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00808080));
    gainSlider->addListener (this);

    addAndMakeVisible (typeComboBox = new ComboBox ("new combo box"));
    typeComboBox->setEditableText (false);
    typeComboBox->setJustificationType (Justification::centredLeft);
    typeComboBox->setTextWhenNothingSelected (String::empty);
    typeComboBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    typeComboBox->addItem (TRANS("Hard clipping"), 1);
    typeComboBox->addItem (TRANS("Soft clipping (poly)"), 2);
    typeComboBox->addItem (TRANS("Soft clipping (exp)"), 3);
    typeComboBox->addItem (TRANS("Full-wave rectifier"), 4);
    typeComboBox->addItem (TRANS("Half-wave rectifier"), 5);
    typeComboBox->addListener (this);


    //[UserPreSize]
    addAndMakeVisible (resizer = new ResizableCornerComponent (this, &resizeLimits));
    resizeLimits.setSizeLimits (150, 150, 850, 290);
    //[/UserPreSize]

    setSize (280, 320);


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

DistortionAudioProcessorEditor::~DistortionAudioProcessorEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    resetButton = nullptr;
    gainSlider = nullptr;
    typeComboBox = nullptr;


    //[Destructor]. You can add your own custom destruction code here..

    //[/Destructor]
}

//==============================================================================
void DistortionAudioProcessorEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff880909));

    g.setColour (Colours::red);
    g.fillEllipse (96.0f, 158.0f, 88.0f, 88.0f);

    g.setColour (Colour (0xff2c333b));
    g.fillEllipse (112.0f, 174.0f, 56.0f, 56.0f);

    g.setColour (Colour (0xff650000));
    g.fillRoundedRectangle (54.0f, 6.0f, 175.0f, 37.0f, 10.000f);

    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 22.30f, Font::bold));
    g.drawText (TRANS("distortion"),
                6, 13, 277, 24,
                Justification::centredTop, true);

    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 15.00f, Font::plain));
    g.drawText (TRANS("GAIN"),
                3, 133, 275, 30,
                Justification::centred, true);

    //[UserPaint] Add your own custom painting code here..

    // Knob bounds
    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 11.00f, Font::plain));
    g.drawText (String(gainSlider->getMinimum()),
                59, 229, 54, 30,
                Justification::centredRight, true);

    g.setColour (Colours::white);
    g.setFont (Font (Font::getDefaultMonospacedFontName(), 11.00f, Font::plain));
    g.drawText (String(gainSlider->getMaximum()),
                168, 229, 54, 30,
                Justification::centredLeft, true);

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

void DistortionAudioProcessorEditor::resized()
{
    resetButton->setBounds (236, 5, 39, 23);
    gainSlider->setBounds (58, 162, 164, 102);
    typeComboBox->setBounds (69, 77, 150, 24);
    //[UserResized] Add your own custom resize handling here..
    getProcessor()->_lastUIWidth = getWidth();
    getProcessor()->_lastUIHeight = getHeight();
    //[/UserResized]
}

void DistortionAudioProcessorEditor::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
	DistortionAudioProcessor* ourProcessor = getProcessor();

    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == resetButton)
    {
        //[UserButtonCode_resetButton] -- add your button handler code here..
		ourProcessor->Reset();
        //[/UserButtonCode_resetButton]
    }

    //[UserbuttonClicked_Post]
	UpdateInterfaceValues();
    //[/UserbuttonClicked_Post]
}

void DistortionAudioProcessorEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    DistortionAudioProcessor* ourProcessor = getProcessor();
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == gainSlider)
    {
        //[UserSliderCode_gainSlider] -- add your slider handling code here..
        ourProcessor->SetGainIndB(gainSlider->getValue());
        //[/UserSliderCode_gainSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void DistortionAudioProcessorEditor::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    DistortionAudioProcessor* ourProcessor = getProcessor();
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == typeComboBox)
    {
        //[UserComboBoxCode_typeComboBox] -- add your combo box handling code here..
        ourProcessor->SetType((DistortionAudioProcessor::Types) typeComboBox->getSelectedItemIndex());
        //[/UserComboBoxCode_typeComboBox]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void DistortionAudioProcessorEditor::SetComboBoxActivity(ComboBox * comboBox, bool active)
{
	if (active){
		comboBox->setColour(ComboBox::backgroundColourId, Colours::white);
	}else{
		comboBox->setColour(ComboBox::backgroundColourId, Colours::grey);
	}
}

void DistortionAudioProcessorEditor::SetSliderActivity(Slider * slider, bool active)
{
	if (active){
		slider->setColour (Slider::rotarySliderFillColourId, Colour (0xffff5c00));
		slider->addListener (this);
	}else{
		slider->setColour (Slider::rotarySliderFillColourId, Colours::grey);
		slider->removeListener(this);
	}
}

void DistortionAudioProcessorEditor::UpdateInterfaceValues()
{
	DistortionAudioProcessor* ourProcessor = getProcessor();

    // Update Sliders
    gainSlider	->setValue(ourProcessor->_gainIndB,dontSendNotification);
    typeComboBox->setItemEnabled((int) ourProcessor->GetType(), true);
}

// This timer periodically checks whether any of the parameters have changed...
void DistortionAudioProcessorEditor::timerCallback()
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

<JUCER_COMPONENT documentType="Component" className="DistortionAudioProcessorEditor"
                 componentName="" parentClasses="public AudioProcessorEditor, public Timer"
                 constructorParams="DistortionAudioProcessor* ownerFilter" variableInitialisers="AudioProcessorEditor (ownerFilter)"
                 snapPixels="8" snapActive="0" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="280" initialHeight="320">
  <BACKGROUND backgroundColour="ff880909">
    <ELLIPSE pos="96 158 88 88" fill="solid: ffff0000" hasStroke="0"/>
    <ELLIPSE pos="112 174 56 56" fill="solid: ff2c333b" hasStroke="0"/>
    <ROUNDRECT pos="54 6 175 37" cornerSize="10" fill="solid: ff650000" hasStroke="0"/>
    <TEXT pos="6 13 277 24" fill="solid: ffffffff" hasStroke="0" text="distortion"
          fontname="Default monospaced font" fontsize="22.300000000000000711"
          bold="1" italic="0" justification="12"/>
    <TEXT pos="3 133 275 30" fill="solid: ffffffff" hasStroke="0" text="GAIN"
          fontname="Default monospaced font" fontsize="15" bold="0" italic="0"
          justification="36"/>
  </BACKGROUND>
  <TEXTBUTTON name="on off button" id="85bb206557e8f33d" memberName="resetButton"
              virtualName="" explicitFocusOrder="0" pos="236 5 39 23" bgColOff="ffad8585"
              bgColOn="ffff0000" buttonText="RESET" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <SLIDER name="new slider" id="b7cd5ae6d0a3bfc6" memberName="gainSlider"
          virtualName="" explicitFocusOrder="0" pos="58 162 164 102" rotarysliderfill="7fffffff"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="808080"
          min="-24" max="24" int="0.010000000000000000208" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="40"
          textBoxHeight="20" skewFactor="1"/>
  <COMBOBOX name="new combo box" id="c80cc28cc68984e4" memberName="typeComboBox"
            virtualName="" explicitFocusOrder="0" pos="69 77 150 24" editable="0"
            layout="33" items="Hard clipping&#10;Soft clipping (poly)&#10;Soft clipping (exp)&#10;Full-wave rectifier&#10;Half-wave rectifier"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
