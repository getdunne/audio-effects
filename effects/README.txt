*** About ***

This repository contains code for AudioUnit and VST plugins using the Juce framework (www.rawmaterialsoftware.com). In addition to the effect source itself, Juce and the VST SDK (2.4) are included to minimise setup time.

This code accompanies the textbook "Audio Effects: Theory, Implementation and Application" by Joshua D. Reiss and Andrew P. McPherson (CRC Press, 2014).

*** Compiling ***

Juce automates the creation of projects/makefiles in various development environments. The file "[name].jucer" in each effect folder contains all the required information to generate a project. These files should be opened in the Introjucer program included in your Juce folder (extras/Introjucer/). Build the Introjucer first and then use it to generate a project in your local development environment. Note that you will need to set the "module path" to your Juce folder within each Introjucer project in order for the projects to compile correctly.

Platform- and environment-specific files (anything in the Builds directory of an effect) should not be included in the repository since Juce automatically regenerates this with every run of the Introjucer.

Mac-specific note: Starting with XCode 4.3, the location of the CoreAudio/AudioUnit files moved from its previous location in /Developer/Extras to a location within the Xcode application bundle (/Applications/Xcode.app/). The Xcode projects Juce creates will not be able to find the required files. The easiest solution for anyone who has installed a previous version of Xcode previously is to create a symbolic link from the files in /Developer/Extras inside the Xcode bundle:

cd /Applications/Xcode.app/Contents/Developer
sudo ln -s /Developer/Extras Extras

On the first compile, a couple errors related to static casting will appear in Apple's AudioUnit files. Add the suggested static_cast fixes and everything should compile normally. (This only has to be done once since the changes are to Apple's Developer/Extras files.)

On a machine where Xcode 4.2 or prior has never been installed, another solution will need to be found.

*** Running ***

By default, the resulting plug-ins will be created as VST on all platforms and additionally AudioUnits on Mac. Any environment that hosts these plugins (e.g. AU Lab on Mac OS X) can be used to test them.