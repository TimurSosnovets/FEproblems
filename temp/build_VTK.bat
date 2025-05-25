@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ============================
echo Setting up Intel oneAPI environment...
echo ============================

call "C:\Program Files (x86)\Intel\oneAPI\setvars.bat"
if errorlevel 1 (
    echo ❌ Failed to set up Intel oneAPI environment!
    exit /b 1
)

echo ============================
echo Creating VTK build directory...
echo ============================

if not exist build (
    mkdir build
)
cd build

echo ============================
echo Running CMake for VTK...
echo ============================

cmake -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_INSTALL_PREFIX=%CD%\..\install ^
  -DVTK_INSTALL_SDK=ON ^
  -DBUILD_SHARED_LIBS=OFF ^
  -DVTK_MODULE_ENABLE_VTK_CommonCore=YES ^
  -DVTK_MODULE_ENABLE_VTK_CommonDataModel=YES ^
  -DVTK_MODULE_ENABLE_VTK_IOXML=YES ^
  -DBUILD_TESTING=OFF ^
  -DBUILD_EXAMPLES=OFF ^
  -DCMAKE_C_COMPILER=icx ^
  -DCMAKE_CXX_COMPILER=icx ^
  ..
if errorlevel 1 (
    echo ❌ CMake configuration for VTK failed!
    exit /b 1
)

echo ============================
echo Building VTK...
echo ============================

cmake --build . --config Release
if errorlevel 1 (
    echo ❌ VTK build failed!
    exit /b 1
)

echo ============================
echo Installing VTK...
echo ============================

cmake --build . --config Release --target install
if errorlevel 1 (
    echo ❌ VTK install failed!
    exit /b 1
)

echo ============================
echo ✅ VTK build and install complete!
echo ============================

pause