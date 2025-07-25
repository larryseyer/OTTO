@echo off
REM Windows Build Script for OTTO
echo Building OTTO for Windows...

REM Create build directory
mkdir build-windows 2>nul
cd build-windows

REM Configure with CMake (using Visual Studio 2022)
cmake .. ^
    -DCMAKE_BUILD_TYPE=Release ^
    -G "Visual Studio 17 2022" ^
    -A x64

REM Build
cmake --build . --config Release --parallel

echo Windows build complete! Check build-windows directory for outputs.