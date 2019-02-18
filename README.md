# Reiss-McPherson Audio Effects examples
This repo contains updated and revised versions of [JUCE](https://juce.com/)-based audio effect plug-in code examples, originally published by Joshua D. Reiss and Andrew McPherson, to accompany their 2014 book [Audio Effects: Theory, Implementation and Application](https://www.crcpress.com/Audio-Effects-Theory-Implementation-and-Application/Reiss-McPherson/p/book/9781466560284). The original published code is hosted at [SoundSoftware](https://code.soundsoftware.ac.uk/projects/audio_effects_textbook_code).

The original code was developed with a much earlier version of the [JUCE](https://juce.com/) software framework than is in use today, since which there have been some API-breaking changes. I am in the process of updating all the examples so they will compile without errors or (eventually) warnings in [Microsoft Visual Studio 2017](https://visualstudio.microsoft.com/) and [Xcode 10](https://developer.apple.com/xcode/).

At the time of writing, the "Robotisation" effect still does not compile, as it relies on older JUCE programming constructs for in-place buffer processing.

This code is licensed under the [GPL3](https://www.gnu.org/licenses/gpl-3.0.en.html).

Note the file README.txt included herewith is from the original code repository and is substantially out-of-date.
