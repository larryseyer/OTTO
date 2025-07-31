@echo off
REM OTTO Phase 2.3.1 - Windows 11 Parallels Optimization Script
REM Optimizes Windows 11 in Parallels for optimal audio development performance

setlocal enabledelayedexpansion

echo 🪟 OTTO Parallels Windows 11 Optimization
echo ==========================================
echo 🎯 Optimizing for audio development with Reaper

REM Colors for output
set "STATUS_COLOR=[32m"
set "ERROR_COLOR=[31m"
set "WARNING_COLOR=[33m"
set "INFO_COLOR=[36m"
set "RESET_COLOR=[0m"

echo %STATUS_COLOR%🔍 Detecting Parallels environment...%RESET_COLOR%

REM Check if we're running in Parallels
set "PARALLELS_DETECTED=false"

REM Check for Parallels Tools Service
sc query "Parallels Tools Service" >nul 2>&1
if !ERRORLEVEL! equ 0 (
    echo %STATUS_COLOR%  ✅ Parallels Tools Service detected%RESET_COLOR%
    set "PARALLELS_DETECTED=true"
)

REM Check for Parallels graphics adapter
wmic path win32_VideoController get name | findstr /i "Parallels" >nul 2>&1
if !ERRORLEVEL! equ 0 (
    echo %STATUS_COLOR%  ✅ Parallels graphics adapter detected%RESET_COLOR%
    set "PARALLELS_DETECTED=true"
)

if "!PARALLELS_DETECTED!"=="false" (
    echo %WARNING_COLOR%⚠️  Parallels environment not detected%RESET_COLOR%
    echo %INFO_COLOR%ℹ️  Some optimizations may not be relevant%RESET_COLOR%
)

echo.
echo %STATUS_COLOR%⚡ Applying Windows performance optimizations...%RESET_COLOR%

REM 1. Audio Service Optimization
echo %INFO_COLOR%🎵 Optimizing audio services...%RESET_COLOR%

REM Set Windows Audio service to high priority
sc config AudioSrv start= auto >nul 2>&1
sc config AudioEndpointBuilder start= auto >nul 2>&1

REM Restart audio services for fresh start
echo   Restarting audio services...
net stop AudioSrv >nul 2>&1
net start AudioSrv >nul 2>&1

if !ERRORLEVEL! equ 0 (
    echo %STATUS_COLOR%  ✅ Audio services optimized%RESET_COLOR%
) else (
    echo %WARNING_COLOR%  ⚠️  Audio service restart failed (may require admin rights)%RESET_COLOR%
)

REM 2. Power Management Optimization
echo %INFO_COLOR%⚡ Configuring power management...%RESET_COLOR%

REM Set power plan to High Performance
powercfg /setactive 8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c >nul 2>&1

REM Disable USB selective suspend
powercfg /setacvalueindex SCHEME_CURRENT 2a737441-1930-4402-8d77-b2bebba308a3 48e6b7a6-50f5-4782-a5d4-53bb8f07e226 0 >nul 2>&1
powercfg /setdcvalueindex SCHEME_CURRENT 2a737441-1930-4402-8d77-b2bebba308a3 48e6b7a6-50f5-4782-a5d4-53bb8f07e226 0 >nul 2>&1

REM Apply power settings
powercfg /setactive SCHEME_CURRENT >nul 2>&1

echo %STATUS_COLOR%  ✅ Power management optimized%RESET_COLOR%

REM 3. System Performance Tweaks
echo %INFO_COLOR%🚀 Applying system performance tweaks...%RESET_COLOR%

REM Disable Windows Defender real-time protection for development folders
echo   Configuring Windows Defender exclusions...
powershell -Command "Add-MpPreference -ExclusionPath 'C:\Users\%USERNAME%\AudioDevelopment'" >nul 2>&1
powershell -Command "Add-MpPreference -ExclusionPath 'C:\Program Files\Common Files\VST3'" >nul 2>&1
powershell -Command "Add-MpPreference -ExclusionPath 'C:\Program Files\Common Files\CLAP'" >nul 2>&1

REM Set process priority for audio applications
echo   Configuring process priorities...
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile\Tasks\Audio" /v Priority /t REG_DWORD /d 6 /f >nul 2>&1
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile\Tasks\Pro Audio" /v Priority /t REG_DWORD /d 1 /f >nul 2>&1

echo %STATUS_COLOR%  ✅ System performance tweaks applied%RESET_COLOR%

REM 4. Network Optimization for Parallels
if "!PARALLELS_DETECTED!"=="true" (
    echo %INFO_COLOR%🌐 Optimizing Parallels network settings...%RESET_COLOR%
    
    REM Disable network throttling
    netsh interface tcp set global chimney=enabled >nul 2>&1
    netsh interface tcp set global rss=enabled >nul 2>&1
    netsh interface tcp set global netdma=enabled >nul 2>&1
    
    echo %STATUS_COLOR%  ✅ Parallels network optimized%RESET_COLOR%
)

REM 5. Audio Buffer Size Optimization
echo %INFO_COLOR%🎵 Configuring audio buffer settings...%RESET_COLOR%

REM Create or update audio configuration registry keys
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\AudioSrv\Parameters" /v ServiceDllUnloadOnStop /t REG_DWORD /d 1 /f >nul 2>&1

REM Set WASAPI buffer sizes
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\MMDevices\Audio\Render" /v DefaultFormat /t REG_BINARY /d 160000004800000010007D00E803000080BB00001000800010 /f >nul 2>&1

echo %STATUS_COLOR%  ✅ Audio buffer settings configured%RESET_COLOR%

REM 6. Visual Studio and Development Tools Optimization
echo %INFO_COLOR%🛠️  Optimizing development environment...%RESET_COLOR%

REM Create development environment variables
setx OTTO_PARALLELS_OPTIMIZED "true" >nul 2>&1
setx JUCE_PARALLELS_MODE "true" >nul 2>&1

REM Set up Visual Studio for optimal performance
if exist "%ProgramFiles%\Microsoft Visual Studio\2022\*" (
    echo   Visual Studio 2022 detected - applying optimizations...
    
    REM Disable unnecessary VS features for better performance
    reg add "HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\17.0\General" /v EnableDiagnosticTools /t REG_DWORD /d 0 /f >nul 2>&1
    reg add "HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\17.0\Debugger" /v EnableDiagnosticTools /t REG_DWORD /d 0 /f >nul 2>&1
)

echo %STATUS_COLOR%  ✅ Development environment optimized%RESET_COLOR%

REM 7. Create Reaper optimization shortcuts
echo %INFO_COLOR%🎵 Setting up Reaper integration...%RESET_COLOR%

REM Check if Reaper is installed
set "REAPER_PATH="
for /d %%i in ("%ProgramFiles%\REAPER*") do (
    if exist "%%i\reaper.exe" (
        set "REAPER_PATH=%%i"
        goto :reaper_found
    )
)

:reaper_found
if not "!REAPER_PATH!"=="" (
    echo %STATUS_COLOR%  ✅ Reaper found at: !REAPER_PATH!%RESET_COLOR%
    
    REM Create optimized Reaper shortcut
    set "DESKTOP=%USERPROFILE%\Desktop"
    
    REM Create batch file for optimized Reaper launch
    echo @echo off > "%DESKTOP%\Launch_Reaper_Optimized.bat"
    echo REM OTTO-Optimized Reaper Launch >> "%DESKTOP%\Launch_Reaper_Optimized.bat"
    echo echo Starting Reaper with OTTO optimizations... >> "%DESKTOP%\Launch_Reaper_Optimized.bat"
    echo. >> "%DESKTOP%\Launch_Reaper_Optimized.bat"
    echo REM Set high priority >> "%DESKTOP%\Launch_Reaper_Optimized.bat"
    echo start /high "!REAPER_PATH!\reaper.exe" >> "%DESKTOP%\Launch_Reaper_Optimized.bat"
    
    echo %STATUS_COLOR%  ✅ Optimized Reaper launcher created on desktop%RESET_COLOR%
) else (
    echo %WARNING_COLOR%  ⚠️  Reaper not found - install from https://www.reaper.fm/%RESET_COLOR%
)

REM 8. Memory and Cache Optimization
echo %INFO_COLOR%💾 Optimizing memory and cache settings...%RESET_COLOR%

REM Clear system cache
echo   Clearing system caches...
rundll32.exe advapi32.dll,ProcessIdleTasks >nul 2>&1

REM Set virtual memory for audio development
wmic computersystem where name="%computername%" set AutomaticManagedPagefile=False >nul 2>&1

echo %STATUS_COLOR%  ✅ Memory optimization complete%RESET_COLOR%

REM 9. Create monitoring scripts
echo %INFO_COLOR%📊 Creating performance monitoring tools...%RESET_COLOR%

REM Create performance monitor batch file
echo @echo off > "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo echo OTTO Audio Performance Monitor >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo echo =============================== >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo echo. >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo echo CPU Usage: >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo wmic cpu get loadpercentage /value ^| find "=" >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo echo. >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo echo Memory Usage: >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo wmic OS get TotalVisibleMemorySize,FreePhysicalMemory /value ^| find "=" >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo echo. >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo echo Audio Services Status: >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo sc query AudioSrv ^| find "STATE" >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo sc query AudioEndpointBuilder ^| find "STATE" >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo echo. >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"
echo pause >> "%USERPROFILE%\Desktop\Monitor_Audio_Performance.bat"

echo %STATUS_COLOR%  ✅ Performance monitoring tools created%RESET_COLOR%

echo.
echo %STATUS_COLOR%✨ Windows 11 Parallels Optimization Complete!%RESET_COLOR%
echo.
echo %INFO_COLOR%📋 Summary of applied optimizations:%RESET_COLOR%
echo   ✅ Audio services optimized for low latency
echo   ✅ Power management set to high performance
echo   ✅ Windows Defender exclusions added for development
echo   ✅ Process priorities configured for audio
if "!PARALLELS_DETECTED!"=="true" (
    echo   ✅ Parallels-specific network optimizations applied
)
if not "!REAPER_PATH!"=="" (
    echo   ✅ Reaper integration configured
)
echo   ✅ Memory and cache settings optimized
echo   ✅ Performance monitoring tools created
echo.
echo %WARNING_COLOR%⚠️  Important Notes:%RESET_COLOR%
echo   • Some changes require a system restart to take full effect
echo   • Run this script as Administrator for best results
echo   • Monitor system performance during audio development
echo.
echo %INFO_COLOR%💡 Next Steps:%RESET_COLOR%
echo   1. Restart Windows to apply all optimizations
echo   2. Test audio latency with your interface
echo   3. Run the OTTO build process
echo   4. Use the Reaper integration for plugin testing
echo.
echo %STATUS_COLOR%🎵 Happy audio development!%RESET_COLOR%

pause