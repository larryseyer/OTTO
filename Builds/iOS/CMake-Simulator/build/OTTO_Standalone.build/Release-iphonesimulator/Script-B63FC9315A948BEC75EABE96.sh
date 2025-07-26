#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/CMake-Simulator
  /opt/homebrew/bin/cmake -E copy /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/CMake-Simulator/OTTO_artefacts/JuceLibraryCode/OTTO_Standalone/PkgInfo /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/Release/Standalone/OTTO.app
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/CMake-Simulator
  /opt/homebrew/bin/cmake -E copy /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/CMake-Simulator/OTTO_artefacts/JuceLibraryCode/OTTO_Standalone/PkgInfo /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/Release/Standalone/OTTO.app
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/CMake-Simulator
  /opt/homebrew/bin/cmake -E copy /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/CMake-Simulator/OTTO_artefacts/JuceLibraryCode/OTTO_Standalone/PkgInfo /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/Release/Standalone/OTTO.app
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/CMake-Simulator
  /opt/homebrew/bin/cmake -E copy /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/CMake-Simulator/OTTO_artefacts/JuceLibraryCode/OTTO_Standalone/PkgInfo /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/Release/Standalone/OTTO.app
fi

