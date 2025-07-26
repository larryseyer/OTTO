#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/JUCE/tools/extras/Build/juceaide/juceaide_artefacts/Custom/juceaide header /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/OTTO_artefacts/JuceLibraryCode/Debug/Defs.txt /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/OTTO_artefacts/JuceLibraryCode/JuceHeader.h
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/JUCE/tools/extras/Build/juceaide/juceaide_artefacts/Custom/juceaide header /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/OTTO_artefacts/JuceLibraryCode/Release/Defs.txt /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/OTTO_artefacts/JuceLibraryCode/JuceHeader.h
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/JUCE/tools/extras/Build/juceaide/juceaide_artefacts/Custom/juceaide header /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/OTTO_artefacts/JuceLibraryCode/MinSizeRel/Defs.txt /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/OTTO_artefacts/JuceLibraryCode/JuceHeader.h
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/JUCE/tools/extras/Build/juceaide/juceaide_artefacts/Custom/juceaide header /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/OTTO_artefacts/JuceLibraryCode/RelWithDebInfo/Defs.txt /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/OTTO_artefacts/JuceLibraryCode/JuceHeader.h
fi

