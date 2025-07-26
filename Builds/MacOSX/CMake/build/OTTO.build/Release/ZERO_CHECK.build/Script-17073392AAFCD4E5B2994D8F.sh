#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  make -f /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  make -f /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  make -f /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake
  make -f /Users/larryseyer/AudioDevelopment/OTTO/Builds/MacOSX/CMake/CMakeScripts/ReRunCMake.make
fi

