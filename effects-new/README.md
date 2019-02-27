# Rebuilt effects
After a thorough review of Reiss and McPherson's code, I have decided to rewrite all of the effects. My main reason is that the original code was written for a much older version of JUCE than is available today, and uses many now-deprecated function calls and coding methods. In a few cases, the code no longer actually works.

A second reason is more personal. I would like these projects to serve as working examples of how *parameters* are managed in JUCE today, including how they are *exposed to a host* (to enable *host automation*), how they are *accessed in a graphical user interface*, and how they are *persisted* across multiple usage sessions (separate instantiations of the plug-ins).

My intent is not to disparage the work of the original authors in any way. All of these considerations were beyond the scope of the original development effort, and were not well provided for by the JUCE framework at the time. The situation is very different today. The JUCE framework has matured enormously, and now provides very good support for plug-in parameter management, but good code examples are exceedingly rare.

It is my sincere hope that by taking the parameter-management bull by the horns, so to speak, I may be able to further Reiss and McPherson's pedagogic goals by equipping students to distinguish the genuinely unique parts of each code project from the surrounding JUCE boilerplate, and at the same time shed a bit of light on how that "boilerplate" ought to be written.

## The BasicGain effect
The **BasicGain** effect is the simplest JUCE audio plug-in which performs a parameter-driven transformation on an audio stream. It's essentially the original "Empty" effect, updated for the JUCE 5.4.x framework.

