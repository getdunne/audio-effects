# Reiss-McPherson Audio Effects examples

This repo contains updated and revised versions of [JUCE](https://juce.com/)-based audio effect plug-in code examples, originally published by Joshua D. Reiss and Andrew McPherson, to accompany their 2014 book [Audio Effects: Theory, Implementation and Application](https://www.crcpress.com/Audio-Effects-Theory-Implementation-and-Application/Reiss-McPherson/p/book/9781466560284).

The original code was developed with a much earlier version of the [JUCE](https://juce.com/) software framework than is in use today, since which there have been some API-breaking changes. I am in the process of re-writing all the examples.

This code is licensed under the [GNU General Public License, version 3](https://www.gnu.org/licenses/gpl-3.0.en.html) (with a few exceptions -- see below under "Licensing").

## Important link
As explained below and elsewhere, I haven't actually completed the re-implementation of the phase vocoder based effects. I would direct the interested reader to [this excellent work](https://github.com/juandagilc/Audio-Effects) by Juan Gil, PhD Fellow at Aalborg University in Denmark.

## Original code
The original code examples are preserved at the [SoundSoftware](https://code.soundsoftware.ac.uk/projects/audio_effects_textbook_code) site. Several of the projects are now substantially out-of-date and don't compile cleanly in [Microsoft Visual Studio 2017](https://visualstudio.microsoft.com/) and [Xcode 10](https://developer.apple.com/xcode/).

The *effects* folder in this repo contains lightly-updated versions of the original projects, which (with one exception) at least compile, though with many warnings. Due to changes in the JUCE framework since the code was written, several of the plug-ins don't actually run properly.

## New code
The *effects-new* folder contains re-written versions of Reiss and McPherson's original examples (plus one or two new ones). I'm doing this in pursuit of five goals:
1. Clean compile and build under VS2017 and Xcode 10.
2. Improved consistency in the code base.
3. Purging all deprecated JUCE coding practices.
4. Improved parameter handling through use of *juce::AudioProcessorValueTreeState* and related classes.
5. Improved graphical user interfaces, which are simple, yet usable.

See the *README.md* files in *effects-new* folder and its subfolders for more details.

## The JUCE framework
The JUCE Framework itself is NOT included here. To use this code, you will need to obtain and install your own copy of the framework from the [JUCE web site](https://juce.com). Your life will be easier if you install the framework in the preferred location for your platform: *C:\\JUCE* on Windows, and *~/JUCE* on macOS and Linux.

Once you have installed the JUCE framework, you should be able to double-click any of the *.jucer* project files to open them in the JUCE *Projucer* application (referred to by its older name, "Introjucer", in the 2014 Reiss-McPherson book), and "export" ready-to-build projects for your IDE(s) from there. Refer to the JUCE site for details.

## Licensing
The JUCE framework is *not free*--it is commercial software which happens to be offered under a dual-license scheme. Its development is funded by people and companies who pay for a *commercial license*, which permits them to use it in *closed-source* projects. However, the developers are kind enough to allow it to be used without payment in *open-source* projects, provided these are offered under the [GNU General Public License, version 3](https://opensource.org/licenses/GPL-3.0) (GPL3). In accordance with this requirement, *all the code examples published here are governed by the GPL3.*

The GPL3 **forbids** *unauthorized* use of GPL-licensed code in closed-source projects, which in turn means that if you copy any part of a GPL-licensed program for use in another project, that project must itself be GPL-licensed and open-source. However, the GPL3 also states clearly that *authorized* uses can be exempt from these rules. This is what allows dual-licensing schemes of the sort used by JUCE, and also allows me to designate certain parts of these projects (of which I am the sole author and copyright holder) as being more freely usable under the [MIT License](https://opensource.org/licenses/MIT).
