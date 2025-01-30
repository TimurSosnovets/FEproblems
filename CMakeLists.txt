cmake_minimum_required(VERSION 3.15)
project(FEProblems)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories for binaries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

# Include directories for headers
include_directories(include)
include_directories(lib/eigen-3.4.0)  # Eigen is header-only

# Add OpenXLSX as a subdirectory
add_subdirectory(lib/OpenXLSX)

# Collect all source files (definitions of classes/methods)
file(GLOB SOURCE_FILES source/*.cpp)

# Specify the test file to compile
set(TEST_FILE tests/Plate_lam_evo_test.cpp)

# Create an executable for the test file
add_executable(FEProblemsTest ${TEST_FILE} ${SOURCE_FILES})

# Link OpenXLSX to the test executable
target_link_libraries(FEProblemsTest OpenXLSX::OpenXLSX)
