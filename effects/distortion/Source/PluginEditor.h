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

#ifndef __JUCE_HEADER_4586FE55775717B6__
#define __JUCE_HEADER_4586FE55775717B6__

//[Headers]     -- You can add your own extra header files here --

//#include "../../Shared/LookAndFeelCustom.h" //used to be _nographics
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class DistortionAudioProcessorEditor  : public AudioProcessorEditor,
                                        public Timer,
                                        public Button::Listener,
                                        public Slider::Listener,
                                        public ComboBox::Listener
{
public:
    //==============================================================================
    DistortionAudioProcessorEditor (DistortionAudioProcessor* ownerFilter);
    ~DistortionAudioProcessorEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void timerCallback();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

    ScopedPointer<ResizableCornerComponent> resizer;
    ComponentBoundsConstrainer resizeLimits;

	//static LookAndFeelCustom* customLookAndFeel;

	AudioPlayHead::CurrentPositionInfo lastDisplayedPosition;

    DistortionAudioProcessor* getProcessor() const
    {
        return static_cast <DistortionAudioProcessor*> (getAudioProcessor());
    }

    void displayPositionInfo (const AudioPlayHead::CurrentPositionInfo& pos);

	void UpdateInterfaceValues();

	void SetComboBoxActivity(ComboBox * comboBox, bool active);
	void SetSliderActivity(Slider * slider, bool active);


    //[/UserVariables]

    //==============================================================================
    ScopedPointer<TextButton> resetButton;
    ScopedPointer<Slider> gainSlider;
    ScopedPointer<ComboBox> typeComboBox;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessorEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_4586FE55775717B6__
