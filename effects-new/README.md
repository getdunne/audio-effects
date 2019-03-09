# Rebuilt effects
After a thorough review of Reiss and McPherson's code, I have decided to rewrite all of the effects. My main reason is that the original code was written for a much older version of JUCE than is available today, and uses many now-deprecated function calls and coding methods. In a few cases, the code no longer actually works.

A second reason is more personal. I would like these projects to serve as working examples of how *parameters* are managed in JUCE today, including how they are *exposed to a host* (to enable *host automation*), how they are *accessed in a graphical user interface*, and how they are *persisted* across multiple usage sessions (separate instantiations of the plug-ins).

My intent is not to disparage the work of the original authors in any way. All of these considerations were beyond the scope of the original development effort, and were not well provided for by the JUCE framework at the time. The situation is very different today. The JUCE framework has matured enormously, and now provides very good support for plug-in parameter management, but good code examples are exceedingly rare.

## Folder and project structure
Each of the rewritten example projects is in a separate folder under this one. I have prepended a two-digit number to the folder names, so they naturally sort into an appropriate order for study--starting with the simplest and proceeding to the more complex.

The *Common* folder contains code which is common to all the individual projects. Much of this code is new, and is provided under the [MIT License](https://opensource.org/licenses/MIT).

## BasicGain
The **BasicGain** effect is the simplest JUCE audio plug-in which performs a parameter-driven transformation on an audio stream. It's essentially the original "Empty" effect, updated for the JUCE 5.4.x framework. Study this code first, to understand the "boilerplate" you'll see (in expanded form) in all of the other projects. See the *README.md* in the *00 - BasicGain* folder for details.

## Effect categories
These effects naturally group into the following taxonomy, roughly in order of increasing complexity:

 * **Amplitude modulation effects** (R&M Chapter 5)
   * BasicGain
   * Tremolo
   * Ring Modulator
   * Compressor

 * **Wave shaping effects** (R&M Chapter 7)
   * Distortion

 * **Filter-based effects** (R&M Chapter 4)
   * Parametric EQ
   * Wah-wah
   * Auto-wah
   * Phaser

 * **Delay-line effects** (R&M Chapter 2)
   * Delay
   * Ping-Pong Delay
   * Vibrato
   * Flanger
   * Chorus
  
 * **Reverberation** (R&M Chapter 11)
   * Reverb

 * **Phase-vocoder effects** (R&M Chapter 8)
   * These two effects are NOT done yet
   * *Pitch shifter*
   * *Robot-voice effect*

In addition to these examples from the Reiss-McPherson text, I have added:

 * **Miscellaneous additional effects** (Not part of the original R&M examples)
   * *"Leslie" Rotary Speaker simulation*

## Notes on the Projucer
The JUCE Projucer is a tremendously useful tool for generating new JUCE projects according to a consistent pattern. It uses code templates to generate "Processor" and "Editor" code files from scratch, but subsequently, you can edit and even rename these files, and the Projucer will leave them alone even if you re-open the *.jucer* file, make configuration changes, and re-save.

Because of this, I tend to use the Projucer to "clone" (copy) and rename an existing project whenever possible. For example, I created the **Tremolo** project by cloning and renaming the **BasicGain** project. I had to rename the "processor", "editor", and "parameters" code files manually, and use global search/replace in a text editor or IDE to rename the corresponding classes and variables, but this approach is still a great time-saver when you are creating a family of highly similar code projects.

When copying a project in order to clone it, you need only copy the *.jucer* file and the *Source* folder. To rename source files in the Projucer, select e.g the *.cpp* file, right-click and choose "rename", edit the name and hit Return/Enter. The Projucer will then ask if you want to rename the *.h* file similarly; confirm that you do.
