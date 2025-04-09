@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ============================
echo Setting up Intel oneAPI environment...
echo ============================

set "VS2022INSTALLDIR=D:\Coding\VisualStudio2022"
call "D:\Coding\Intel\oneAPI\setvars.bat"

echo ============================
echo Creating build directory...
echo ============================

if not exist build (
    mkdir build
)
cd build

echo ============================
echo Running CMake with MSVC and Intel compiler...
echo ============================

cmake -G "Visual Studio 17 2022" -T "IntelLLVM" -A x64 ..

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