@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ============================
echo Setting up Intel oneAPI environment...
echo ============================

:: Initialize Intel oneAPI environment - now showing output for debugging
call "C:\Program Files (x86)\Intel\oneAPI\setvars.bat"
if errorlevel 1 (
    echo ❌ Failed to initialize Intel oneAPI environment
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

:: Clear cache to ensure fresh configuration
if exist CMakeCache.txt del CMakeCache.txt

:: Configure with explicit MKL paths
cmake -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DMKL_ROOT="C:/Program Files (x86)/Intel/oneAPI/mkl/latest" ^
    -DMKL_INCLUDE_DIR="C:/Program Files (x86)/Intel/oneAPI/mkl/latest/include" ^
    ..

if errorlevel 1 (
    echo ❌ CMake configuration failed!
    exit /b 1
)

echo ============================
echo Building the project...
echo ============================

:: Build with maximum available processors and detailed logging
cmake --build . --config Release -- /m:8 /v:detailed

if errorlevel 1 (
    echo ❌ Build failed!
    
    :: Show MKL-related environment variables for debugging
    echo.
    echo MKL Environment Variables:
    set MKL
    set MKLROOT
    
    exit /b 1
)

echo ============================
echo ✅ Build succeeded!
echo ============================

:: Copy output files
if exist ".\output\*.exe" (
    echo Copying executables to project root...
    copy ".\output\*.exe" "..\"
)

pause