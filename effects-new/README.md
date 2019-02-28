# Rebuilt effects
After a thorough review of Reiss and McPherson's code, I have decided to rewrite all of the effects. My main reason is that the original code was written for a much older version of JUCE than is available today, and uses many now-deprecated function calls and coding methods. In a few cases, the code no longer actually works.

A second reason is more personal. I would like these projects to serve as working examples of how *parameters* are managed in JUCE today, including how they are *exposed to a host* (to enable *host automation*), how they are *accessed in a graphical user interface*, and how they are *persisted* across multiple usage sessions (separate instantiations of the plug-ins).

My intent is not to disparage the work of the original authors in any way. All of these considerations were beyond the scope of the original development effort, and were not well provided for by the JUCE framework at the time. The situation is very different today. The JUCE framework has matured enormously, and now provides very good support for plug-in parameter management, but good code examples are exceedingly rare.

It is my sincere hope that by taking the parameter-management bull by the horns, so to speak, I may be able to further Reiss and McPherson's pedagogic goals by equipping students to distinguish the genuinely unique parts of each code project from the surrounding JUCE boilerplate, and at the same time shed a bit of light on how that "boilerplate" ought to be written.

## The BasicGain effect
The **BasicGain** effect is the simplest JUCE audio plug-in which performs a parameter-driven transformation on an audio stream. It's essentially the original "Empty" effect, updated for the JUCE 5.4.x framework. Study this code first, to understand the "boilerplate" you'll see (in expanded form) in all of the other projects.

Parameter handling in JUCE plug-ins is best done using **juce::AudioProcessorValueTreeState** and numerous related classes. These appear to have had a long and troublesome gestation, but now seem ready for regular use. The basic approach is:
1. Add an **AudioProcessorValueTreeState** data member to the main **AudioProcessor**-derived class.
2. Initialize this member in the initializer list for the main AudioProcessor's constructor, linking it to the AudioProcessor object itself, an optional **UndoManager** object (in *BasicGain*, also a data member of the AudioProcessor), and an **AudioProcessorValueTreeState::ParameterLayout** instance, which defines all of the actual parameters.
3. Derive some classes from **AudioProcessorValueTreeState::Listener**, and attach them to individual parameters, so as to update an ordinary variable (float, int, etc.) which the main AudioProcessor's *processBlock()* function can use.
4. Use helper classes like **AudioProcessorValueTreeState::SliderAttachment** to connect each parameter to the corresponding control widget in the plug-in's GUI.

In all the updated audio effect projects, steps 2, 3, and 4 above are handled by a special "parameters" class; in *BasicGain* this is called *GainParameters*.
* A **GainParameters** data member called *parameters* is added to the main AudioProcessor.
* It has ordinary member variables which contain the "working values" of the plug-in parameters. In *BasicGain* there is only one parameter, *Gain*, which is decibel-valued, but the "working value" member *linearGain* holds the converted value in the range 0.0 to 1.0.
* The decibel-to-linear-gain conversion is handled by the class **GainParameters::FloatDecibelListener**, specifically in its *parameterChanged()* callback function.

It's worth understanding that *BasicGain*'s single *Gain* parameter can have three distinct value ranges, which are used in different contexts:
1. The *user* will see only the decibel values, which may range from -100 dB to +12 dB.
2. The audio-processing code sees only the *linearGain* variable, which gets "magically" updated with the equivalent linear gain value every time the *Gain* parameter is changed, through the attached *FloatDecibelListener* object, whose *parameterChanged()* callback simply calls the utility function *juce::Decibels::decibelsToGain()*.
3. The *plug-in host* sees a "normalized" representation of the dB values, mapped onto the range 0.0 to 1.0. (This is a simple linear mapping, NOT dB-to-linear-gain conversion.) This value normalization is an aspect of how JUCE handles and hides the differences among different plug-in standards (e.g. VST2/3, AU, etc.).

In an *AudioProcessorValueTreeState*, every plug-in parameter has three unique *juce::String*s, which are defined as `static const String` in the "parameters" class:
1. The "ID" string (in this case "gain") identifies the parameter. It serves as a lookup key in the *AudioProcessorValueTreeState::ParameterLayout* data structure.
2. The "name" string (in this case "Gain") is a user-friendly name which will be displayed by the host.
3. The "label" string is essentially for units of measure, e.g. "dB" for decibels.

**AudioProcessorValueTreeState** defines several "attachment" classes, which effectively link ("attach") parameters to GUI controls. In *BasicGain*, a single **SliderAttachment** object is used. Because this object must be created when the GUI is instantiated, and destroyed when the GUI is deleted, a *std::unique_ptr* member (called *gainAttachment*) is used. The *GainParameters* class has function members *attachControls()* and *detachControls()* which manage this creation/destruction of attachment objects.

The *BasicGain* effect has only a single float-valued parameter, but the basic code structure of the *GainParameters* class can easily be extended to handle many parameters of various types. All of the effect projects have a comparably-structured "parameteters" class.
