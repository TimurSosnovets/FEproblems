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

# Find Intel MKL (assumes oneAPI is installed)
find_path(MKL_INCLUDE_DIR mkl.h
    HINTS "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/include")
find_library(MKL_CORE_LIB mkl_core
    HINTS "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/lib/intel64")
find_library(MKL_INTEL_LP64_LIB mkl_intel_lp64
    HINTS "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/lib/intel64")
find_library(MKL_SEQUENTIAL_LIB mkl_sequential
    HINTS "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/lib/intel64")

if(MKL_INCLUDE_DIR AND MKL_CORE_LIB)
    include_directories(${MKL_INCLUDE_DIR})
    set(MKL_LIBRARIES ${MKL_INTEL_LP64_LIB} ${MKL_SEQUENTIAL_LIB} ${MKL_CORE_LIB})
    add_definitions(-DEIGEN_USE_MKL_ALL)  # Enable MKL in Eigen
else()
    message(FATAL_ERROR "Intel MKL not found. Please install Intel oneAPI Base Toolkit.")
endif()

# Add OpenXLSX as a subdirectory
add_subdirectory(lib/OpenXLSX)

# Collect all source files (definitions of classes/methods)
file(GLOB SOURCE_FILES source/*.cpp)

# Collect all test files (*.cpp) from tests/actual/ folder
file(GLOB TEST_FILES tests/actual/*.cpp)

# Create an executable for each test file
foreach(TEST_FILE ${TEST_FILES})
    # Extract the filename without extension (e.g., Plate_lam_evo_test)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    # Create an executable for each test
    add_executable(${TEST_NAME} ${TEST_FILE} ${SOURCE_FILES})

    # Link OpenXLSX and MKL to each test executable
    target_link_libraries(${TEST_NAME} OpenXLSX::OpenXLSX ${MKL_LIBRARIES})
endforeach()

# Optional: Set Intel icx as the compiler (uncomment if desired)
# set(CMAKE_C_COMPILER "icx")
# set(CMAKE_CXX_COMPILER "icx")