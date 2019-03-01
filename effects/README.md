# Reiss-McPherson Audio Effects examples
This folder contains very slightly updated versions of the audio effects code examples, originally published by Joshua D. Reiss and Andrew McPherson, to accompany their 2014 book [Audio Effects: Theory, Implementation and Application](https://www.crcpress.com/Audio-Effects-Theory-Implementation-and-Application/Reiss-McPherson/p/book/9781466560284). The original published code is hosted at [SoundSoftware](https://code.soundsoftware.ac.uk/projects/audio_effects_textbook_code).

The original code was developed with a much earlier version of the [JUCE](https://juce.com/) software framework than is in use today, since which there have been some API-breaking changes. **I am in the process of re-writing all these examples**; see the *effects-new* folder.

This code is licensed under the [GPL3](https://www.gnu.org/licenses/gpl-3.0.en.html).

Note the file README.txt included herewith is from the original code repository and is substantially out-of-date.

## FFTW required for Phase Vocoder examples
The Phase Vocoder example projects *pvoc_passthrough*, *pvoc_pitchshift*, and *robotisation* projects all require the [fftw library](http://www.fftw.org).

On the Mac, you can just follow the [Installation on Unix](http://www.fftw.org/fftw3_doc/Installation-on-Unix.html#Installation-on-Unix) instructions. Make sure to do the "install" step as *root* (e.g., use `su`).

On Windows, I built the fftw library at *C:\fftw-3.3.5-dll64*, and set the Visual Studio *Header Search Paths* and *Extra Library Search Paths* to that folder path in the Projucer.

## Open problems
At the time of writing, the *robotisation* project still does not compile, as it relies on older JUCE programming constructs for in-place buffer processing.

Also, the *distortion*, *pvoc_passthrough* and *pvoc_pitchshift* projects compile, but the resulting Mac Audio Unit (AU) plug-ins do not pass validation in Logic Pro X. *distortion* works OK as a VST on Windows, but the two *pvoc* VSTs crash.

Other issues noted on Mac with Logic Pro X:

* The *reverb* AU plugin passes validation, but crashes Logic Pro X when used.
* The *compressor* doesn't seem to have any effect (odd because the VST version works on Windows)


