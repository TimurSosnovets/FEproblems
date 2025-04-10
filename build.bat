@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ============================
echo Setting up environment...
echo ============================

:: Option 1: Use MSVC environment (Visual Studio 2022)


:: Option 2: Use Intel oneAPI environment (uncomment to use icx instead of MSVC)
call "C:\Program Files (x86)\Intel\oneAPI\setvars.bat"
if errorlevel 1 (
     echo ❌ Failed to set up Intel oneAPI environment!
     exit /b 1
)

echo ============================
echo Creating build directory...
echo ============================

if not exist build (
    mkdir build
)
cd build

echo ============================
echo Running CMake with MSVC...
echo ============================

:: Generate with Visual Studio 2022 (MSVC)
cmake -G "Visual Studio 17 2022" -A x64 ..
if errorlevel 1 (
    echo ❌ CMake configuration failed!
    exit /b 1
)

echo ============================
echo Building the project...
echo ============================

cmake --build . --config Release
if errorlevel 1 (
    echo ❌ Build failed!
    exit /b 1
)

echo ============================
echo ✅ Build complete!
echo ============================

pause