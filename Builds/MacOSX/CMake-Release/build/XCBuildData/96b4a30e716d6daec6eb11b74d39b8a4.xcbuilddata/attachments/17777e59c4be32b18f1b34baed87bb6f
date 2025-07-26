#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /opt/homebrew/bin/cmake -E copy /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/OTTO_artefacts/JuceLibraryCode/OTTO_AU/PkgInfo /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/AU/OTTO.component/Contents
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/AU/OTTO.component -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/AU/OTTO.component -Ddest=/Users/larryseyer/Library/Audio/Plug-Ins/Components -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /opt/homebrew/bin/cmake -E copy /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/OTTO_artefacts/JuceLibraryCode/OTTO_AU/PkgInfo /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/AU/OTTO.component/Contents
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/AU/OTTO.component -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/AU/OTTO.component -Ddest=/Users/larryseyer/Library/Audio/Plug-Ins/Components -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /opt/homebrew/bin/cmake -E copy /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/OTTO_artefacts/JuceLibraryCode/OTTO_AU/PkgInfo /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/AU/OTTO.component/Contents
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/AU/OTTO.component -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/AU/OTTO.component -Ddest=/Users/larryseyer/Library/Audio/Plug-Ins/Components -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /opt/homebrew/bin/cmake -E copy /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release/OTTO_artefacts/JuceLibraryCode/OTTO_AU/PkgInfo /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/AU/OTTO.component/Contents
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/AU/OTTO.component -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake-Release
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/AU/OTTO.component -Ddest=/Users/larryseyer/Library/Audio/Plug-Ins/Components -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/copyDir.cmake
fi

