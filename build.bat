@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ============================
echo Setting up environment...
echo ============================

:: Clear build folder (uncomment if needed)
::rmdir /s /q build

:: Use Intel oneAPI environment (for icx compiler and MKL)
call "C:\Program Files (x86)\Intel\oneAPI\setvars.bat"
if errorlevel 1 (
    echo ❌ Failed to set up Intel oneAPI environment!
    exit /b 1
)

:: Set MKL to use all available threads (16 for Ryzen 7 5800X3D)
set MKL_NUM_THREADS=8
:: Optimize MKL for AMD CPUs
set MKL_DEBUG_CPU_TYPE=5
:: Ensure OpenMP library path is included
set LIB=%LIB%;C:\Program Files (x86)\Intel\oneAPI\compiler\latest\windows\lib

echo ============================
echo Creating build directory...
echo ============================

if not exist build (
    mkdir build
)
cd build

echo ============================
echo Running CMake with Intel icx...
echo ============================

:: Generate with Visual Studio 2022 (using icx)
cmake -G "Visual Studio 17 2022" -A x64 -DVTK_DIR=%CD%\..\lib\vtk\lib\cmake\vtk-9.4 ..
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