/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Compressor: dynamic range compression effect
  See textbook Chapter 6: Dynamics Processing
 
  Code by Joshua Reiss, Brecht de Man and Andrew McPherson
 
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

#ifndef __JUCER_HEADER_COMPRESSORAUDIOPROCESSOREDITOR_PLUGINEDITOR_75AA535A__
#define __JUCER_HEADER_COMPRESSORAUDIOPROCESSOREDITOR_PLUGINEDITOR_75AA535A__

//[Headers]     -- You can add your own extra header files here --
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class CompressorAudioProcessorEditor  : public AudioProcessorEditor,
                                        public Timer,
                                        public Button::Listener,
                                        public Slider::Listener
{
public:
    //==============================================================================
    CompressorAudioProcessorEditor (CompressorAudioProcessor* ownerFilter);
    ~CompressorAudioProcessorEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void timerCallback();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void sliderValueChanged (Slider* sliderThatWasMoved);

    // Binary resources:
    static const char* brushedMetalDark_jpg;
    static const int brushedMetalDark_jpgSize;
    static const char* c4dm_png2;
    static const int c4dm_png2Size;
    static const char* qmul_png2;
    static const int qmul_png2Size;
    static const char* knobstrip_png;
    static const int knobstrip_pngSize;
    static const char* scaleLr_png;
    static const int scaleLr_pngSize;


    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

    ScopedPointer<ResizableCornerComponent> resizer;
    ComponentBoundsConstrainer resizeLimits;



	AudioPlayHead::CurrentPositionInfo lastDisplayedPosition;

    CompressorAudioProcessor* getProcessor() const
    {
        return static_cast <CompressorAudioProcessor*> (getAudioProcessor());
    }

    void displayPositionInfo (const AudioPlayHead::CurrentPositionInfo& pos);

    //[/UserVariables]

    //==============================================================================
    TextButton* buttonONOFF;
    Label* label;
    Slider* sliderThreshold;
    Label* label2;
    Slider* sliderRatio;
    Label* label3;
    Slider* sliderGain;
    Label* label7;
    Slider* sliderAttack;
    Label* label5;
    Slider* sliderRelease;
    Label* label6;
    Image cachedImage_brushedMetalDark_jpg;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CompressorAudioProcessorEditor (const CompressorAudioProcessorEditor&);
    const CompressorAudioProcessorEditor& operator= (const CompressorAudioProcessorEditor&);
};


#endif   // __JUCER_HEADER_COMPRESSORAUDIOPROCESSOREDITOR_PLUGINEDITOR_75AA535A__
