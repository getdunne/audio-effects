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

#ifndef __JUCE_HEADER_3DB9693590AE4D1__
#define __JUCE_HEADER_3DB9693590AE4D1__

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
class ReverbAudioProcessorEditor  : public AudioProcessorEditor,
                                    public Timer,
                                    public Slider::Listener
{
public:
    //==============================================================================
    ReverbAudioProcessorEditor (ReverbAudioProcessor* ownerFilter);
    ~ReverbAudioProcessorEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void timerCallback();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

    ScopedPointer<ResizableCornerComponent> resizer;
    ComponentBoundsConstrainer resizeLimits;

	//static LookAndFeelCustom* customLookAndFeel;

	AudioPlayHead::CurrentPositionInfo lastDisplayedPosition;

    ReverbAudioProcessor* getProcessor() const
    {
        return static_cast <ReverbAudioProcessor*> (getAudioProcessor());
    }

    void displayPositionInfo (const AudioPlayHead::CurrentPositionInfo& pos);

	void UpdateInterfaceValues();

	void SetComboBoxActivity(ComboBox * comboBox, bool active);
	void SetSliderActivity(Slider * slider, bool active);


    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Slider> lateEarlySlider;
    ScopedPointer<Slider> mixSlider;
    ScopedPointer<Slider> gainSlider;
    ScopedPointer<Slider> preDelaySlider;
    ScopedPointer<Slider> bandWidthSlider;
    ScopedPointer<Slider> dampSlider;
    ScopedPointer<Slider> sizeSlider;
    ScopedPointer<Slider> decaySlider;
    ScopedPointer<Slider> densitySlider;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbAudioProcessorEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_3DB9693590AE4D1__
