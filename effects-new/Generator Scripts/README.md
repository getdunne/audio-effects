# Code-generation scripts

Python scripts for template-based generation of starter code for new audio-effects projects. These are based on the [Jinja2](http://jinja.pocoo.org/) template library.

Writing and using scripts to generate code is extremely demanding work, and **I do not recommend this practice** for any but the most experienced programmers. This is simply an approach I have chosen to use, and it's better to include all this in the open-source repo than to leave it out.

## Setup

Works fine in the Python v2.7 installed by default on macOS Mojave (or Windows, or Linux), but you'll need to install Jinja2 as
```
sudo easy_install Jinja2
```

Actually I did
```
sudo easy_install pip
sudo pip Jinja2
```
because I'm used to *pip*.

## Use

Next you'll need to edit *Parameters.xslx* (in Excel), re-export to *Parameters.csv*, and edit *gen.py* to set the project name (on or about line 33). Once you've done all that, run (in this directory):
```
python gen.py
```
This will create six source files, which are starter code for your new "parameters", "processor", and "editor" classes.

These template-generated files will get you 90% of the way to working code. You'll get some compile errors, because the templates aren't perfect.
