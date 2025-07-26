#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/CMake-Simulator
  echo Build\ all\ projects
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/CMake-Simulator
  echo Build\ all\ projects
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/CMake-Simulator
  echo Build\ all\ projects
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/larryseyer/AudioDevelopment/OTTO/Builds/iOS/CMake-Simulator
  echo Build\ all\ projects
fi

