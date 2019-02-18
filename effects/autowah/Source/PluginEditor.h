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

#ifndef __JUCE_HEADER_FEB21157F431A7A8__
#define __JUCE_HEADER_FEB21157F431A7A8__

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
class AutoWahAudioProcessorEditor  : public AudioProcessorEditor,
                                     public Timer,
                                     public Slider::Listener
{
public:
    //==============================================================================
    AutoWahAudioProcessorEditor (AutoWahAudioProcessor* ownerFilter);
    ~AutoWahAudioProcessorEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void timerCallback();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    ScopedPointer<ResizableCornerComponent> resizer_;
    ComponentBoundsConstrainer resizeLimits_;

    //static LookAndFeelCustom* customLookAndFeel;

    AudioPlayHead::CurrentPositionInfo lastDisplayedPosition;

    AutoWahAudioProcessor* getProcessor() const
    {
        return static_cast <AutoWahAudioProcessor*> (getAudioProcessor());
    }

    void displayPositionInfo (const AudioPlayHead::CurrentPositionInfo& pos);

	void UpdateInterfaceValues();

	void SetComboBoxActivity(ComboBox * comboBox, bool active);
	void SetSliderActivity(Slider * slider, bool active);


    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Slider> baseFrequencySlider_;
    ScopedPointer<Slider> qSlider_;
    ScopedPointer<Slider> lfoFrequencySlider_;
    ScopedPointer<Slider> lfoWidthSlider_;
    ScopedPointer<Slider> envelopeWidthSlider_;
    ScopedPointer<Slider> envelopeAttackSlider_;
    ScopedPointer<Slider> envelopeDecaySlider_;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoWahAudioProcessorEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_FEB21157F431A7A8__
