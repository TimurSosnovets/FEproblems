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
include_directories(lib/Eigen)  # Eigen is header-only

# Add OpenXLSX as a subdirectory
add_definitions(-D__MINGW32__)
add_subdirectory(lib/OpenXLSX)

# Collect all source files (definitions of classes/methods)
file(GLOB SOURCE_FILES source/*.cpp)

# Collect all test files (*.cpp) from tests/ folder
file(GLOB TEST_FILES tests/*.cpp)

# Create an executable for each test file
foreach(TEST_FILE ${TEST_FILES})
    # Extract the filename without extension (e.g., Plate_lam_evo_test)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    # Create an executable for each test
    add_executable(${TEST_NAME} ${TEST_FILE} ${SOURCE_FILES})

    # Link OpenXLSX to each test executable
    target_link_libraries(${TEST_NAME} OpenXLSX::OpenXLSX)
endforeach()
