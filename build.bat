@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ============================
echo Creating build directory...
echo ============================

if not exist build (
    mkdir build
)
cd build

echo ============================
echo Running CMake with MinGW...
echo ============================

cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..
if errorlevel 1 (
    echo ❌ CMake configuration failed!
    exit /b 1
)

echo ============================
echo Building the project...
echo ============================

mingw32-make
if errorlevel 1 (
    echo ❌ Build failed!
    exit /b 1
)

echo ============================
echo ✅ Build complete!
echo ============================

pause
