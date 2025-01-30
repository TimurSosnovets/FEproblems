@echo off
echo Creating build directory...
if not exist build (
    mkdir build
)
cd build

echo Running CMake with Visual Studio generator...
cmake -G "Visual Studio 17 2022" ..

echo Building the project in Release mode...
cmake --build . --config Release

echo Build complete!
pause
