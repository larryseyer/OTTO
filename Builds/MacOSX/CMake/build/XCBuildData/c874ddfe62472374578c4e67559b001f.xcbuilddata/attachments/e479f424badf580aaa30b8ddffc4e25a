#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -E copy /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake/OTTO_artefacts/JuceLibraryCode/OTTO_VST3/PkgInfo /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -E echo removing\ moduleinfo.json
  /opt/homebrew/bin/cmake -E remove -f /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/moduleinfo.json /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources/moduleinfo.json
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -E echo "creating /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources/moduleinfo.json"
  /opt/homebrew/bin/cmake -E make_directory /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources
  /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake/Debug/juce_vst3_helper -create -version 1.0.0 -path /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -output /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources/moduleinfo.json
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -Ddest=/Users/larryseyer/Library/Audio/Plug-Ins/VST3 -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -E copy /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake/OTTO_artefacts/JuceLibraryCode/OTTO_VST3/PkgInfo /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -E echo removing\ moduleinfo.json
  /opt/homebrew/bin/cmake -E remove -f /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/moduleinfo.json /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources/moduleinfo.json
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -E echo "creating /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources/moduleinfo.json"
  /opt/homebrew/bin/cmake -E make_directory /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources
  /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake/Release/juce_vst3_helper -create -version 1.0.0 -path /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -output /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources/moduleinfo.json
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -Ddest=/Users/larryseyer/Library/Audio/Plug-Ins/VST3 -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -E copy /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake/OTTO_artefacts/JuceLibraryCode/OTTO_VST3/PkgInfo /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -E echo removing\ moduleinfo.json
  /opt/homebrew/bin/cmake -E remove -f /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/moduleinfo.json /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources/moduleinfo.json
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -E echo "creating /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources/moduleinfo.json"
  /opt/homebrew/bin/cmake -E make_directory /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources
  /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake/MinSizeRel/juce_vst3_helper -create -version 1.0.0 -path /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -output /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources/moduleinfo.json
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -Ddest=/Users/larryseyer/Library/Audio/Plug-Ins/VST3 -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -E copy /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake/OTTO_artefacts/JuceLibraryCode/OTTO_VST3/PkgInfo /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -E echo removing\ moduleinfo.json
  /opt/homebrew/bin/cmake -E remove -f /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/moduleinfo.json /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources/moduleinfo.json
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -E echo "creating /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources/moduleinfo.json"
  /opt/homebrew/bin/cmake -E make_directory /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources
  /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake/RelWithDebInfo/juce_vst3_helper -create -version 1.0.0 -path /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -output /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3/Contents/Resources/moduleinfo.json
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  /opt/homebrew/bin/cmake -Dsrc=/Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/Release/VST3/OTTO.vst3 -Ddest=/Users/larryseyer/Library/Audio/Plug-Ins/VST3 -P /Users/larryseyer/AudioDevelopment/OTTO/JUCE/extras/Build/CMake/copyDir.cmake
fi

