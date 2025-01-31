@echo off
echo Creating build directory...
if not exist build (
    mkdir build
)
cd build

echo Running CMake with MinGW...
cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..

echo Building the project...
mingw32-make

echo Build complete!
pause